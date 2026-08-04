#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderProgram.h>

#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/BowOGL3xRenderContext.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xStorageBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xUniformBuffer.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xFragmentOutputs.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderObject.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderResource.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLShaderProgram::OGLShaderProgram(const std::string &VertexShaderSource, const std::string &GeometryShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource)
    : m_ShaderProgramHandle(0)
{
    FN("OGLShaderProgram::OGLShaderProgram");

    LOG_TRACE("glCreateProgram");
    m_ShaderProgramHandle = glCreateProgram();
    if (m_ShaderProgramHandle == 0)
        LOG_ERROR("Could not create Shaderobject.");

    m_ready = false;
    m_vertexShader = OGLShaderObjectPtr(new OGLShaderObject(GL_VERTEX_SHADER, VertexShaderSource));
    if (GeometryShaderSource.length() > 0)
    {
        m_geometryShader = OGLShaderObjectPtr(new OGLShaderObject(GL_GEOMETRY_SHADER, GeometryShaderSource));
    }
    m_fragmentShader = OGLShaderObjectPtr(new OGLShaderObject(GL_FRAGMENT_SHADER, FragmentShaderSource));

    LOG_TRACE("glAttachShader");
    glAttachShader(m_ShaderProgramHandle, m_vertexShader->GetShader());
    if (GeometryShaderSource.length() > 0)
    {
        LOG_TRACE("glAttachShader");
        glAttachShader(m_ShaderProgramHandle, m_geometryShader->GetShader());
    }
    LOG_TRACE("glAttachShader");
    glAttachShader(m_ShaderProgramHandle, m_fragmentShader->GetShader());

    LOG_TRACE("glLinkProgram");
    glLinkProgram(m_ShaderProgramHandle);

    int linkStatus;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == 0)
    {
        LOG_ERROR(GetLog().c_str());
        m_ready = false;
    }
    else
    {
        LOG_TRACE("Shader successfully linked!");
        m_ready = true;
    }

    m_fragmentOutputs = OGLFragmentOutputsPtr(new OGLFragmentOutputs(m_ShaderProgramHandle));
    m_shaderVertexAttributes = FindVertexAttributes(m_ShaderProgramHandle);

    m_shaderResources = FindShaderResources(m_ShaderProgramHandle);
    m_storageBuffers = FindStorageBuffers();
    m_uniformBuffers = FindUniformBuffers();
}

OGLShaderProgram::~OGLShaderProgram()
{
    FN("OGLShaderProgram::~OGLShaderProgram");

    LOG_TRACE("glDeleteProgram");
    glDeleteProgram(m_ShaderProgramHandle);
}

ShaderVertexAttributePtr OGLShaderProgram::VGetVertexAttribute(std::string name)
{
    FN("OGLShaderProgram::VGetVertexAttribute");

    for (auto it = m_shaderVertexAttributes.begin(); it != m_shaderVertexAttributes.end(); it++)
    {
        if (it->second->Name == name)
        {
            return it->second;
        }
    }
    return nullptr;
}

ShaderVertexAttributeMap OGLShaderProgram::VGetVertexAttributes()
{
    FN("OGLShaderProgram::VGetVertexAttributes");

    return m_shaderVertexAttributes;
}

int OGLShaderProgram::VGetFragmentOutputLocation(std::string name)
{
    FN("OGLShaderProgram::VGetFragmentOutputLocation");

    return (*m_fragmentOutputs)[name];
}

ShaderResourceBindingsPtr OGLShaderProgram::VCreateResourceBindingObjects()
{
    FN("OGLShaderProgram::VCreateResourceBindingObjects");

    LOG_FATAL("Not yet Implemented");
    return nullptr;
}

std::string OGLShaderProgram::GetLog()
{
    FN("OGLShaderProgram::GetLog");

    char *buffer = nullptr;
    GLint length, result;

    /* get the shader info log */
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_INFO_LOG_LENGTH, &length);
    buffer = (char *)malloc(length);

    LOG_TRACE("glGetProgramInfoLog");
    glGetProgramInfoLog(m_ShaderProgramHandle, length, &result, buffer);

    if (result == GL_FALSE)
    {
        LOG_ERROR("Could not get shader info log!");
        free(buffer);
        return std::string();
    }

    if (buffer != nullptr)
        return std::string(buffer);
    else
        return std::string();
}

uint32_t OGLShaderProgram::GetProgram()
{
    FN("OGLShaderProgram::GetProgram");

    return m_ShaderProgramHandle;
}

void OGLShaderProgram::Bind()
{
    FN("OGLShaderProgram::Bind");

    for (auto it = m_storageBuffers.begin(); it != m_storageBuffers.end(); it++)
    {
        if (it->second.Buffer != nullptr)
        {
            it->second.Buffer->Bind(it->second.Binding);
        }
    }

    for (auto it = m_uniformBuffers.begin(); it != m_uniformBuffers.end(); it++)
    {
        if (it->second.Buffer != nullptr)
        {
            it->second.Buffer->Bind(it->second.Binding);
        }
    }

    LOG_TRACE("glUseProgram");
    glUseProgram(m_ShaderProgramHandle);
}

bool OGLShaderProgram::IsReady()
{
    FN("OGLShaderProgram::IsReady");

    return m_ready;
}

// =====================================================================
// PRIVATE FUNCTIONS
// =====================================================================

ShaderVertexAttributeMap OGLShaderProgram::FindVertexAttributes(uint32_t programHandle)
{
    FN("OGLShaderProgram::FindVertexAttributes");

    int numberOfAttributes;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &numberOfAttributes);

    int attributeNameMaxLength;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeNameMaxLength);

    ShaderVertexAttributeMap vertexAttributes = ShaderVertexAttributeMap();

    if (numberOfAttributes > 0)
        LOG_TRACE("\tVertexAttributes:");

    for (size_t i = 0; i < (uint32_t)numberOfAttributes; ++i)
    {
        int attributeNameLength;
        int attributeLength;
        GLenum attributeType;
        GLchar *attributeName = new GLchar[attributeNameMaxLength];

        LOG_TRACE("glGetActiveAttrib");
        glGetActiveAttrib(programHandle, i, attributeNameMaxLength, &attributeNameLength, &attributeLength, &attributeType, attributeName);

        if (strncmp(attributeName, "gl_", strlen("gl_")) == 0)
        {
            //
            // Names starting with the reserved prefix of "gl_" have a location
            // of -1.
            continue;
        }
        LOG_TRACE("glGetAttribLocation");
        int attributeLocation = glGetAttribLocation(programHandle, attributeName);

        LOG_TRACE("\t\tName: %s, \tLocation: %d", attributeName, attributeLocation);
        vertexAttributes.insert(std::make_pair(attributeLocation, ShaderVertexAttributePtr(new ShaderVertexAttribute(attributeLocation, std::string(attributeName), OGLTypeConverter::ToActiveAttribType(attributeType), attributeLength))));
    }
    return vertexAttributes;
}

ShaderResourceMap OGLShaderProgram::FindShaderResources(uint32_t program)
{
    FN("OGLShaderProgram::FindResources");
    ShaderResourceMap shaderResouceMap;
    LOG_FATAL("Not yet Implemented");
    return shaderResouceMap;
}

std::unordered_map<std::string, ShaderUniformBufferUnit> OGLShaderProgram::FindUniformBuffers()
{
    FN("OGLComputeShaderProgram::FindUniformBuffers");

    std::unordered_map<std::string, ShaderUniformBufferUnit> buffers;

    GLint numShaderResourceBlocks;

    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_UNIFORM_BLOCKS, &numShaderResourceBlocks);
    LOG_INFO("Number of active uniform blocks: %d", numShaderResourceBlocks);

    for (GLint i = 0; i < numShaderResourceBlocks; ++i)
    {
        char name[256];
        GLsizei length;
        GLint binding;
        GLint size;

        LOG_TRACE("glGetActiveUniformBlockName");
        glGetActiveUniformBlockName(m_ShaderProgramHandle, i, sizeof(name), &length, name);

        LOG_TRACE("glGetActiveUniformBlockiv");
        glGetActiveUniformBlockiv(m_ShaderProgramHandle, i, GL_UNIFORM_BLOCK_BINDING, &binding);

        LOG_TRACE("glGetActiveUniformBlockiv");
        glGetActiveUniformBlockiv(m_ShaderProgramHandle, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        LOG_INFO("ShaderResource Block %d: %s (Binding: %d)", i, name, binding);

        std::string name_string = std::string(name);
        buffers.insert(std::pair<std::string, ShaderUniformBufferUnit>(name_string, ShaderUniformBufferUnit(name_string, binding, size, OGLShaderResourceBufferPtr(nullptr))));
    }
    return buffers;
}

std::unordered_map<std::string, ShaderStorageBufferUnit> OGLShaderProgram::FindStorageBuffers()
{
    FN("OGLComputeShaderProgram::FindUniformBuffers");

    std::unordered_map<std::string, ShaderStorageBufferUnit> buffers;
    GLint numStorageBlocks;

    LOG_TRACE("glGetProgramInterfaceiv");
    glGetProgramInterfaceiv(m_ShaderProgramHandle, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numStorageBlocks);
    LOG_INFO("Number of active shader storage blocks: %d", numStorageBlocks);

    GLenum properties[] = {GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE};
    for (GLint i = 0; i < numStorageBlocks; ++i)
    {
        GLint results[3];

        LOG_TRACE("glGetProgramResourceiv");
        glGetProgramResourceiv(m_ShaderProgramHandle, GL_SHADER_STORAGE_BLOCK, i, 3, properties, 3, nullptr, results);

        std::string name(results[0], '\0');

        LOG_TRACE("glGetProgramResourceName");
        glGetProgramResourceName(m_ShaderProgramHandle, GL_SHADER_STORAGE_BLOCK, i, results[0], nullptr, &name[0]);

        LOG_INFO("Shader Storage Block %d: %s (Binding: %d, Size: %d)", i, name.c_str(), results[1], results[2]);

        std::string name_string = std::string(name.c_str());
        buffers.insert(std::pair<std::string, ShaderStorageBufferUnit>(name_string, ShaderStorageBufferUnit(name_string, results[1], results[2], OGLStorageBufferPtr(nullptr))));
    }
    return buffers;
}
/*
void OGLShaderProgram::VSetBuffer(const char *name, UniformBufferPtr uniformBuffer)
{
    FN("OGLShaderProgram::VSetBuffer");

    std::string name_string = std::string(name);
    if (m_uniformBuffers.find(name_string) != m_uniformBuffers.end())
    {
        m_uniformBuffers[name_string].Buffer = std::dynamic_pointer_cast<OGLShaderResourceBuffer>(uniformBuffer);
        return;
    }
    else
    {
        LOG_ERROR("Could not find uniform buffer with name: %s", name);
    }
}

void OGLShaderProgram::VSetBuffer(const char *name, StorageBufferPtr storageBuffer)
{
    FN("OGLShaderProgram::VSetBuffer");

    std::string name_string = std::string(name);
    if (m_storageBuffers.find(name_string) != m_storageBuffers.end())
    {
        m_storageBuffers[name_string].Buffer = std::dynamic_pointer_cast<OGLStorageBuffer>(storageBuffer);
        return;
    }
    else
    {
        LOG_ERROR("Could not find storage buffer with name: %s", name);
    }
}

void OGLShaderProgram::VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler)
{
    FN("OGLShaderProgram::VSetTexture");

    LOG_FATAL("Not yet Implemented");
}
*/
void OGLShaderProgram::VSetPushConstants(const char *name, const void *data, size_t offset, size_t size)
{
    FN("OGLShaderProgram::VSetPushConstants");

    LOG_FATAL("Not yet Implemented");
}

void OGLShaderProgram::VSetPushConstants(ShaderStage shaderStage, const void *data, size_t offset, size_t size)
{
    FN("OGLShaderProgram::VSetPushConstants");

    LOG_FATAL("Not yet Implemented");
}

} // namespace bow
