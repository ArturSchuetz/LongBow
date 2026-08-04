#include <OpenGL3xRenderDevice/Device/BowOGL3xRenderContext.h>

#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xIndexBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xStorageBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xUniformBuffer.h>
#include <OpenGL3xRenderDevice/Device/Context/FrameBuffer/BowOGL3xFramebuffer.h>
#include <OpenGL3xRenderDevice/Device/Context/VertexAttributeBindings/BowOGL3xVertexAttributeBindings.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderProgram.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderResourceBindings.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTexture2D.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTextureSampler.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTextureUnits.h>

#include <RenderDevice/BowClearState.h>
#include <RenderDevice/Device/Context/Mesh/BowMeshBuffers.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/Geometry/BowMeshAttribute.h>

#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

namespace bow
{

OGLRenderContext *OGLRenderContext::m_currentContext;

namespace
{

//! Routes driver diagnostics into the engine log.
/*!
    Without this the only way to notice a bad enum, an incomplete framebuffer
    or a shader falling back to software is to call glGetError by hand after
    every entry point. The driver knows all of it already; this asks it to say
    so, with severity mapped onto the engine's own levels.
*/
void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar *message, const void * /*userParam*/)
{
    const char *sourceName = "other";
    switch (source)
    {
    case GL_DEBUG_SOURCE_API: sourceName = "api"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceName = "window system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceName = "shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: sourceName = "third party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: sourceName = "application"; break;
    default: break;
    }

    const char *typeName = "other";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: typeName = "error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeName = "deprecated"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeName = "undefined behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY: typeName = "portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: typeName = "performance"; break;
    case GL_DEBUG_TYPE_MARKER: typeName = "marker"; break;
    default: break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        LOG_ERROR("OpenGL [%s/%s/%u]: %s", sourceName, typeName, id, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOG_WARNING("OpenGL [%s/%s/%u]: %s", sourceName, typeName, id, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        LOG_WARNING("OpenGL [%s/%s/%u]: %s", sourceName, typeName, id, message);
        break;
    default:
        LOG_TRACE("OpenGL [%s/%s/%u]: %s", sourceName, typeName, id, message);
        break;
    }
}

} // namespace

OGLRenderContext::OGLRenderContext(GLFWwindow *window)
    : m_viewport(), m_clearColor(), m_clearDepth(1.0f), m_clearStencil(0), m_renderState(), m_boundShaderProgram(nullptr), m_textureUnits(nullptr), m_boundFramebuffer(nullptr), m_setFramebuffer(nullptr), m_window(window), m_device(nullptr),
      m_versionMajor(0), m_versionMinor(0), m_hasDirectStateAccess(false), m_lineProgram(0), m_lineVertexBuffer(0), m_lineVertexArray(0),
      m_initialized(false), m_vsync(false)
{
    FN("OGLRenderContext::OGLRenderContext");
}

bool OGLRenderContext::Initialize(OGLRenderDevice *device)
{
    FN("OGLRenderContext::Initialize");

    m_device = device;

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    LOG_TRACE("glewInit");
    if (GLEW_OK != glewInit())
    {
        LOG_ERROR("Could not initialize GLEW!");
    }
    else
    {
        LOG_TRACE("GLEW sucessfully initialized!");
    }

    Vector4<float> clearColor;
    LOG_TRACE("glGetFloatv");
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &m_clearDepth);
    LOG_TRACE("glGetIntegerv");
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &m_clearStencil);
    LOG_TRACE("glGetFloatv");
    glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat *)&clearColor);

    m_clearColor = clearColor * 255.0f;

    m_textureUnits = OGLTextureUnitsPtr(new OGLTextureUnits());

    //
    // Sync GL state with default render state.
    //
    ForceApplyRenderState(m_renderState);

    int width, height;
    LOG_TRACE("glfwGetFramebufferSize");
    glfwGetFramebufferSize(m_window, &width, &height);
    VSetViewport(Viewport(0, 0, width, height));

    LOG_TRACE("OpenGL-Context sucessfully initialized!");

    // Checking GL version
    LOG_TRACE("glGetString");
    const GLubyte *GLVersionString = glGetString(GL_VERSION);
    LOG_INFO("OpenGL %s, %s, %s", GLVersionString, glGetString(GL_RENDERER), glGetString(GL_VENDOR));

    glGetIntegerv(GL_MAJOR_VERSION, &m_versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &m_versionMinor);

    // Direct state access became core in 4.5. Paths that would otherwise call
    // glNamedBufferData and friends check this and fall back to bind-to-edit.
    m_hasDirectStateAccess = (m_versionMajor > 4) || (m_versionMajor == 4 && m_versionMinor >= 5);
    if (!m_hasDirectStateAccess)
    {
        LOG_WARNING("OpenGL %d.%d has no core direct state access; falling back to bind-to-edit.", m_versionMajor, m_versionMinor);
    }

    if (glDebugMessageCallback != nullptr)
    {
        LOG_TRACE("glDebugMessageCallback");
        glEnable(GL_DEBUG_OUTPUT);
        // Synchronous delivery costs performance but makes the callback fire
        // on the call that caused the problem, so the log points at the right
        // place instead of somewhere later in the frame.
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        // Notifications are the driver chatting about buffer memory; the log
        // is verbose enough already.
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    }

    // Only GL_CLAMP_READ_COLOR survives in the core profile. The vertex and
    // fragment variants were removed along with the fixed-function pipeline
    // and had been raising GL_INVALID_ENUM here on every startup, unnoticed
    // until the debug callback above started reporting it.
    LOG_TRACE("glClampColor");
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);

    m_initialized = true;
    return m_initialized;
}

OGLRenderContext::~OGLRenderContext()
{
    FN("OGLRenderContext::~OGLRenderContext");

    VRelease();
}

void OGLRenderContext::VRelease()
{
    FN("OGLRenderContext::VRelease");

    m_initialized = false;
    m_window = nullptr;
    LOG_TRACE("OGLRenderContext released");
}

VertexAttributeBindingsPtr OGLRenderContext::VCreateVertexAttributeBindings(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
{
    FN("OGLRenderContext::VCreateVertexAttributeBindings");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    return VCreateVertexAttributeBindings(m_device->VCreateMeshBuffers(mesh, shaderAttributes, usageHint));
}

VertexAttributeBindingsPtr OGLRenderContext::VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers)
{
    FN("OGLRenderContext::VCreateVertexAttributeBindings");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    VertexAttributeBindingsPtr vertexAttributeBindings = VCreateVertexAttributeBindings();
    if (meshBuffers->IndexBuffer != nullptr)
    {
        vertexAttributeBindings->VSetIndexBuffer(meshBuffers->IndexBuffer);
    }

    VertexBufferAttributeMap attributeMap = meshBuffers->GetAttributes();
    for (auto attribute = attributeMap.begin(); attribute != attributeMap.end(); ++attribute)
    {
        vertexAttributeBindings->VSetAttribute(attribute->first, attribute->second);
    }

    return vertexAttributeBindings;
}

VertexAttributeBindingsPtr OGLRenderContext::VCreateVertexAttributeBindings()
{
    FN("OGLRenderContext::VCreateVertexAttributeBindings");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    return VertexAttributeBindingsPtr(new OGLVertexAttributeBindings());
}

FramebufferPtr OGLRenderContext::VCreateFramebuffer()
{
    FN("OGLRenderContext::VCreateFramebuffer");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    return OGLFramebufferPtr(new OGLFramebuffer());
}

void OGLRenderContext::VBeginFrame() {}

void OGLRenderContext::VEndFrame() {}

void OGLRenderContext::VClear(ClearState clearState)
{
    FN("OGLRenderContext::VClear");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    ApplyFramebuffer();

    ApplyScissorTest(clearState.scissorTest);
    ApplyColorMask(clearState.colorMask);
    ApplyDepthMask(clearState.depthMask);
    // TODO: StencilMaskSeparate

    if (m_clearColor != clearState.color)
    {
        LOG_TRACE("glClearColor!");
        glClearColor(clearState.color.a[0], clearState.color.a[1], clearState.color.a[2], clearState.color.a[3]);
        m_clearColor = clearState.color;
    }

    if (m_clearDepth != clearState.depth)
    {
        LOG_TRACE("glClearDepth");
        glClearDepth((float)clearState.depth);
        m_clearDepth = clearState.depth;
    }

    if (m_clearStencil != clearState.stencil)
    {
        LOG_TRACE("glClearStencil");
        glClearStencil(clearState.stencil);
        m_clearStencil = clearState.stencil;
    }

    LOG_TRACE("glClear");
    glClear(OGLTypeConverter::To(clearState.buffers));
}

void OGLRenderContext::VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("OGLRenderContext::VDraw");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    ApplyRenderState(renderState);
    ApplyVertexAttributeBindings(vertexAttributeBindings);
    ApplyShaderProgram(shaderProgram);

    Draw(primitiveType, 0, std::dynamic_pointer_cast<OGLVertexAttributeBindings>(vertexAttributeBindings)->MaximumArrayIndex() + 1, vertexAttributeBindings, shaderProgram, renderState);
}

void OGLRenderContext::VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("OGLRenderContext::VDraw");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    ApplyRenderState(renderState);
    ApplyVertexAttributeBindings(vertexAttributeBindings);
    ApplyShaderProgram(shaderProgram);

    Draw(primitiveType, offset, count, vertexAttributeBindings, shaderProgram, renderState);
}

void OGLRenderContext::VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("OGLRenderContext::VDraw");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    ApplyRenderState(renderState);
    ApplyVertexAttributeBindings(vertexAttributeBindings);
    // The program has to be current before its sampler uniforms can be
    // assigned, so the resource bindings are applied after it, not before.
    ApplyShaderProgram(shaderProgram);
    ApplyShaderResourceBindings(shaderResourceBindings, shaderProgram);

    Draw(primitiveType, offset, count, vertexAttributeBindings, shaderProgram, renderState);
}

void OGLRenderContext::VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end)
{
    FN("OGLRenderContext::VDrawLine");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    m_textureUnits->Clean();

    ApplyFramebuffer();

    // glBegin/glVertex/glEnd was removed with the core profile. The line goes
    // through a small dedicated vertex buffer and shader instead, both created
    // on first use so a program that never draws a line pays nothing.
    if (!EnsureLineResources())
    {
        return;
    }

    const float vertices[6] = {start.x, start.y, start.z, end.x, end.y, end.z};

    LOG_TRACE("glNamedBufferSubData");
    glNamedBufferSubData(m_lineVertexBuffer, 0, sizeof(vertices), vertices);

    LOG_TRACE("glUseProgram");
    glUseProgram(m_lineProgram);
    LOG_TRACE("glBindVertexArray");
    glBindVertexArray(m_lineVertexArray);
    LOG_TRACE("glDrawArrays");
    glDrawArrays(GL_LINES, 0, 2);
    LOG_TRACE("glBindVertexArray");
    glBindVertexArray(0);

    // The shader program the caller had bound is no longer current.
    m_boundShaderProgram = nullptr;
}

bool OGLRenderContext::EnsureLineResources()
{
    FN("OGLRenderContext::EnsureLineResources");

    if (m_lineProgram != 0)
    {
        return true;
    }

    if (!m_hasDirectStateAccess)
    {
        LOG_ERROR("VDrawLine needs OpenGL 4.5 or newer.");
        return false;
    }

    static const char *vertexSource = "#version 450 core\n"
                                      "layout(location = 0) in vec3 inPosition;\n"
                                      "void main() { gl_Position = vec4(inPosition, 1.0); }\n";

    static const char *fragmentSource = "#version 450 core\n"
                                        "layout(location = 0) out vec4 outColor;\n"
                                        "void main() { outColor = vec4(1.0); }\n";

    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    m_lineProgram = glCreateProgram();
    glAttachShader(m_lineProgram, vertexShader);
    glAttachShader(m_lineProgram, fragmentShader);
    glLinkProgram(m_lineProgram);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(m_lineProgram, GL_LINK_STATUS, &linkStatus);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (linkStatus == GL_FALSE)
    {
        LOG_ERROR("Could not link the line shader used by VDrawLine.");
        glDeleteProgram(m_lineProgram);
        m_lineProgram = 0;
        return false;
    }

    glCreateBuffers(1, &m_lineVertexBuffer);
    glNamedBufferData(m_lineVertexBuffer, sizeof(float) * 6, nullptr, GL_DYNAMIC_DRAW);

    glCreateVertexArrays(1, &m_lineVertexArray);
    glVertexArrayVertexBuffer(m_lineVertexArray, 0, m_lineVertexBuffer, 0, sizeof(float) * 3);
    glEnableVertexArrayAttrib(m_lineVertexArray, 0);
    glVertexArrayAttribFormat(m_lineVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_lineVertexArray, 0, 0);

    return true;
}
/*
void OGLRenderContext::VSetTexture(uint32_t textureId, Texture2DPtr texture)
{
    FN("OGLRenderContext::VSetTexture");

    LOG_ASSERT(textureId < m_textureUnits->GetMaxTextureUnits(), "TextureUnit does not Exist");

    m_textureUnits->SetTexture(textureId, std::dynamic_pointer_cast<OGLTexture2D>(texture));
}

void OGLRenderContext::VSetTextureSampler(uint32_t textureId, TextureSamplerPtr sampler)
{
    FN("OGLRenderContext::VSetTextureSampler");

    m_textureUnits->SetSampler(textureId, std::dynamic_pointer_cast<OGLTextureSampler>(sampler));
}
*/
void OGLRenderContext::VSetFramebuffer(FramebufferPtr framebufer)
{
    FN("OGLRenderContext::VSetFramebuffer");

    m_setFramebuffer = std::dynamic_pointer_cast<OGLFramebuffer>(framebufer);
}

void OGLRenderContext::VSetViewport(Viewport viewport)
{
    FN("OGLRenderContext::VSetViewport");

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    LOG_ASSERT(!(viewport.width < 0 || viewport.height < 0), "The viewport width and height must be greater than or equal to zero.");

    if (m_viewport != viewport)
    {
        m_viewport = viewport;
        LOG_TRACE("glViewport");
        glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    }
}

Viewport OGLRenderContext::VGetViewport()
{
    FN("OGLRenderContext::VGetViewport");

    return m_viewport;
}

void OGLRenderContext::VSwapBuffers(bool vsync)
{
    FN("OGLRenderContext::VSwapBuffers");
    OPTICK_EVENT();

    if (m_currentContext != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentContext = this;
    }

    if (m_vsync != vsync)
    {
        if (vsync)
        {
            LOG_TRACE("glfwSwapInterval");
            glfwSwapInterval(1);
        }
        else
        {
            LOG_TRACE("glfwSwapInterval");
            glfwSwapInterval(0);
        }
        m_vsync = vsync;
    }

    LOG_TRACE("glfwSwapBuffers");
    glfwSwapBuffers(m_window);
}

void OGLRenderContext::VTraceRays(void * /*shaderProgram*/, ShaderResourceBindingsPtr /*resourceBindings*/, Texture2DPtr /*outputImage*/, uint32_t /*width*/, uint32_t /*height*/)
{
    FN("OGLRenderContext::VTraceRays");
    OPTICK_EVENT();

    LOG_FATAL("Ray Tracing is not supported in OpenGL!");
}

// =================================================================================================
// Private Methods
// =================================================================================================

void OGLRenderContext::Draw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("OGLRenderContext::Draw");

    m_textureUnits->Clean();

    LOG_TRACE("glFlush");
    glFlush();
    ApplyFramebuffer();

    OGLVertexAttributeBindingsPtr oglVertexAttributeBindings = std::dynamic_pointer_cast<OGLVertexAttributeBindings>(vertexAttributeBindings);
    OGLIndexBufferPtr oglIndexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(oglVertexAttributeBindings->VGetIndexBuffer());

    if (oglIndexBuffer != nullptr)
    {
        if (offset == 0 && count == oglVertexAttributeBindings->MaximumArrayIndex() + 1)
        {
            LOG_TRACE("glDrawRangeElements");
            glDrawRangeElements(OGLTypeConverter::To(primitiveType), 0, oglVertexAttributeBindings->MaximumArrayIndex(), oglIndexBuffer->GetCount(), OGLTypeConverter::To(oglIndexBuffer->GetDatatype()), 0);
        }
        else
        {
            LOG_TRACE("glDrawRangeElements");
            glDrawRangeElements(OGLTypeConverter::To(primitiveType), 0, oglVertexAttributeBindings->MaximumArrayIndex(), count, OGLTypeConverter::To(oglIndexBuffer->GetDatatype()), (void *)(offset * sizeof(uint32_t)));
        }
    }
    else
    {
        LOG_TRACE("glDrawArrays");
        glDrawArrays(OGLTypeConverter::To(primitiveType), offset, count);
    }
}

void OGLRenderContext::ApplyVertexAttributeBindings(VertexAttributeBindingsPtr vertexAttributeBindings)
{
    FN("OGLRenderContext::ApplyVertexAttributeBindings");

    OGLVertexAttributeBindingsPtr oglVertexAttributeBindings = std::dynamic_pointer_cast<OGLVertexAttributeBindings>(vertexAttributeBindings);
    oglVertexAttributeBindings->Bind();
    oglVertexAttributeBindings->Clean();
}

void OGLRenderContext::ApplyShaderResourceBindings(ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram)
{
    FN("OGLRenderContext::ApplyShaderResourceBindings");
    OPTICK_EVENT();

    if (shaderResourceBindings == nullptr)
    {
        return;
    }

    OGLShaderResourceBindingsPtr bindings = std::dynamic_pointer_cast<OGLShaderResourceBindings>(shaderResourceBindings);
    OGLShaderProgramPtr program = std::dynamic_pointer_cast<OGLShaderProgram>(shaderProgram);
    if (bindings == nullptr || program == nullptr)
    {
        LOG_ERROR("Resource bindings or shader program were not created by the OpenGL device.");
        return;
    }

    const uint32_t programHandle = program->GetProgram();
    const int maxTextureUnits = m_textureUnits->GetMaxTextureUnits();

    // Texture units are handed out in the order the bindings were recorded.
    // OpenGL addresses a sampler by the unit its uniform points at, so the
    // assignment only has to be consistent within one draw.
    int nextTextureUnit = 0;

    for (const auto &entry : bindings->GetTextures())
    {
        LOG_TRACE("glGetUniformLocation");
        const GLint location = glGetUniformLocation(programHandle, entry.first.c_str());
        if (location < 0)
        {
            LOG_WARNING("Shader has no sampler named '%s'; the bound texture is ignored.", entry.first.c_str());
            continue;
        }

        if (nextTextureUnit >= maxTextureUnits)
        {
            LOG_ERROR("Out of texture units: this GPU offers %d and the draw needs more.", maxTextureUnits);
            break;
        }

        m_textureUnits->SetTexture(nextTextureUnit, entry.second.texture);
        if (entry.second.sampler != nullptr)
        {
            m_textureUnits->SetSampler(nextTextureUnit, entry.second.sampler);
        }

        LOG_TRACE("glUniform1i");
        glUniform1i(location, nextTextureUnit);

        ++nextTextureUnit;
    }

    for (const auto &entry : bindings->GetUniformBuffers())
    {
        LOG_TRACE("glGetUniformBlockIndex");
        const GLuint blockIndex = glGetUniformBlockIndex(programHandle, entry.first.c_str());
        if (blockIndex == GL_INVALID_INDEX)
        {
            LOG_WARNING("Shader has no uniform block named '%s'; the bound buffer is ignored.", entry.first.c_str());
            continue;
        }

        LOG_TRACE("glUniformBlockBinding");
        glUniformBlockBinding(programHandle, blockIndex, blockIndex);
        entry.second->Bind(blockIndex);
    }

    for (const auto &entry : bindings->GetStorageBuffers())
    {
        LOG_TRACE("glGetProgramResourceIndex");
        const GLuint blockIndex = glGetProgramResourceIndex(programHandle, GL_SHADER_STORAGE_BLOCK, entry.first.c_str());
        if (blockIndex == GL_INVALID_INDEX)
        {
            LOG_WARNING("Shader has no storage block named '%s'; the bound buffer is ignored.", entry.first.c_str());
            continue;
        }

        LOG_TRACE("glShaderStorageBlockBinding");
        glShaderStorageBlockBinding(programHandle, blockIndex, blockIndex);
        entry.second->Bind(blockIndex);
    }
}

void OGLRenderContext::ApplyShaderProgram(ShaderProgramPtr shaderProgram)
{
    FN("OGLRenderContext::ApplyShaderProgram");

    OGLShaderProgramPtr oglShaderProgram = std::dynamic_pointer_cast<OGLShaderProgram>(shaderProgram);

    if (m_boundShaderProgram != oglShaderProgram)
    {
        oglShaderProgram->Bind();
        m_boundShaderProgram = oglShaderProgram;
    }

#if _DEBUG
    LOG_TRACE("glValidateProgram");
    glValidateProgram(m_boundShaderProgram->GetProgram());

    int validateStatus;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_boundShaderProgram->GetProgram(), GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == 0)
    {
        LOG_ERROR("Shader program validation failed: %s", m_boundShaderProgram->GetLog().c_str());
    }
#endif
}

void OGLRenderContext::ApplyFramebuffer()
{
    FN("OGLRenderContext::ApplyFramebuffer");

    if (m_boundFramebuffer != m_setFramebuffer)
    {
        if (m_setFramebuffer != nullptr)
        {
            m_setFramebuffer->Bind();
        }
        else
        {
            OGLFramebuffer::UnBind();
        }

        m_boundFramebuffer = m_setFramebuffer;
    }

    if (m_setFramebuffer != nullptr)
    {
        m_setFramebuffer->Clean();
#if _DEBUG
        LOG_TRACE("glCheckFramebufferStatus");
        GLenum errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        LOG_ASSERT(!(errorCode != GL_FRAMEBUFFER_COMPLETE), "Frame buffer is incomplete.");
#endif
    }
}

} // namespace bow
