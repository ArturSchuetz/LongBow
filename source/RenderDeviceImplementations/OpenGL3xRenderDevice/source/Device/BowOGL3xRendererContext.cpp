#include <OpenGL3xRenderDevice/Device/BowOGL3xRenderContext.h>

#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xIndexBuffer.h>
#include <OpenGL3xRenderDevice/Device/Context/FrameBuffer/BowOGL3xFramebuffer.h>
#include <OpenGL3xRenderDevice/Device/Context/VertexAttributeBindings/BowOGL3xVertexAttributeBindings.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderProgram.h>
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

OGLRenderContext::OGLRenderContext(GLFWwindow *window)
    : m_viewport(), m_clearColor(), m_clearDepth(1.0f), m_clearStencil(0), m_renderState(), m_boundShaderProgram(nullptr), m_textureUnits(nullptr), m_boundFramebuffer(nullptr), m_setFramebuffer(nullptr), m_window(window), m_device(nullptr),
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
    LOG_TRACE("Using GL_VERSION: %s", GLVersionString);

    LOG_TRACE("glClampColor");
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    LOG_TRACE("glClampColor");
    glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
    LOG_TRACE("glClampColor");
    glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);

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
    LOG_FATAL("ApplyShaderResourceBindings not implemented!");
    // ApplyShaderResourceBindings(shaderResourceBindings);
    ApplyShaderProgram(shaderProgram);

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

    LOG_TRACE("glFlush");
    glFlush();
    ApplyFramebuffer();

    LOG_TRACE("glBegin");
    glBegin(GL_LINES);
    LOG_TRACE("glVertex3f");
    glVertex3f(start.x, start.y, start.z);
    LOG_TRACE("glVertex3f");
    glVertex3f(end.x, end.y, end.z);
    LOG_TRACE("glEnd");
    glEnd();
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
