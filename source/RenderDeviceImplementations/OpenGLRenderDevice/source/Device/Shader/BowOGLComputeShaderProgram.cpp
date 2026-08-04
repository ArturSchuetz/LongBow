#include <OpenGLRenderDevice/Device/Shader/BowOGLComputeShaderProgram.h>

#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderResourceBindings.h>

#include <OpenGLRenderDevice/BowOGLTypeConverter.h>
#include <OpenGLRenderDevice/Device/BowOGLRenderContext.h>
#include <OpenGLRenderDevice/Device/Buffer/BowOGLStorageBuffer.h>
#include <OpenGLRenderDevice/Device/Buffer/BowOGLUniformBuffer.h>
#include <OpenGLRenderDevice/Device/Shader/BowOGLFragmentOutputs.h>
#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderObject.h>
#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderResource.h>

#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLComputeShaderProgram::OGLComputeShaderProgram(const std::string &computeShaderSource)
    : m_computeShader(nullptr), m_fragmentOutputs(nullptr), m_shaderVertexAttributes(ShaderVertexAttributeMap()), m_uniforms(ShaderResourceMap()), m_uniformBuffers(0), m_storageBuffers(0), m_dirtyShaderResources(std::list<ICleanable *>()),
      m_ready(false), m_ShaderProgramHandle(0)
{
    FN("OGLComputeShaderProgram::OGLComputeShaderProgram");

    LOG_TRACE("glewIsSupported");
    if (!glewIsSupported("GL_ARB_shader_storage_buffer_object"))
    {
        LOG_ERROR("GL_ARB_shader_storage_buffer_object not supported!");
        m_ready = false;
        return;
    }

    LOG_TRACE("glCreateProgram");
    m_ShaderProgramHandle = glCreateProgram();
    if (m_ShaderProgramHandle == 0)
        LOG_ERROR("Could not create Shaderobject.");

    m_ready = false;
    m_computeShader = OGLShaderObjectPtr(new OGLShaderObject(GL_COMPUTE_SHADER, computeShaderSource));

    LOG_TRACE("glAttachShader");
    glAttachShader(m_ShaderProgramHandle, m_computeShader->GetShader());
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
    m_shaderVertexAttributes = FindVertexAttributes();

    m_uniforms = FindShaderResources();
    m_storageBuffers = FindStorageBuffers();
    m_uniformBuffers = FindUniformBuffers();
}

OGLComputeShaderProgram::~OGLComputeShaderProgram()
{
    FN("OGLComputeShaderProgram::~OGLComputeShaderProgram");

    LOG_TRACE("glDeleteProgram");
    glDeleteProgram(m_ShaderProgramHandle);
}

ShaderResourceBindingsPtr OGLComputeShaderProgram::VCreateComputeResourceBindingObjects()
{
    FN("OGLComputeShaderProgram::VCreateComputeResourceBindingObjects");
    OPTICK_EVENT();

    // As in the graphics path, there is no driver-side object to allocate:
    // OpenGL binds against the program that is current at dispatch time.
    return OGLShaderResourceBindingsPtr(new OGLShaderResourceBindings());
}

void OGLComputeShaderProgram::VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    FN("OGLComputeShaderProgram::VDispatch");
    OPTICK_EVENT();

    Clean();

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

    Bind();

    // Buffers handed in through the resource bindings are bound against the
    // block index the program reports for them, so a caller can address them
    // by the name used in the shader instead of a hardcoded slot.
    OGLShaderResourceBindingsPtr bindings = std::dynamic_pointer_cast<OGLShaderResourceBindings>(shaderResourceBindings);
    if (bindings != nullptr)
    {
        for (const auto &entry : bindings->GetStorageBuffers())
        {
            LOG_TRACE("glGetProgramResourceIndex");
            const GLuint blockIndex = glGetProgramResourceIndex(m_ShaderProgramHandle, GL_SHADER_STORAGE_BLOCK, entry.first.c_str());
            if (blockIndex == GL_INVALID_INDEX)
            {
                LOG_WARNING("Compute shader has no storage block named '%s'; the bound buffer is ignored.", entry.first.c_str());
                continue;
            }

            LOG_TRACE("glShaderStorageBlockBinding");
            glShaderStorageBlockBinding(m_ShaderProgramHandle, blockIndex, blockIndex);
            entry.second->Bind(blockIndex);
        }

        for (const auto &entry : bindings->GetUniformBuffers())
        {
            LOG_TRACE("glGetUniformBlockIndex");
            const GLuint blockIndex = glGetUniformBlockIndex(m_ShaderProgramHandle, entry.first.c_str());
            if (blockIndex == GL_INVALID_INDEX)
            {
                LOG_WARNING("Compute shader has no uniform block named '%s'; the bound buffer is ignored.", entry.first.c_str());
                continue;
            }

            LOG_TRACE("glUniformBlockBinding");
            glUniformBlockBinding(m_ShaderProgramHandle, blockIndex, blockIndex);
            entry.second->Bind(blockIndex);
        }
    }

    LOG_TRACE("glDispatchCompute");
    glDispatchCompute(groupCountX, groupCountY, groupCountZ);

    LOG_TRACE("glMemoryBarrier");
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void OGLComputeShaderProgram::NotifyDirty(ICleanable *value)
{
    FN("OGLComputeShaderProgram::NotifyDirty");

    m_dirtyShaderResources.push_back(value);
}

std::string OGLComputeShaderProgram::GetLog()
{
    FN("OGLComputeShaderProgram::GetLog");

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

uint32_t OGLComputeShaderProgram::GetProgram()
{
    FN("OGLComputeShaderProgram::GetProgram");

    return m_ShaderProgramHandle;
}

void OGLComputeShaderProgram::Bind()
{
    FN("OGLComputeShaderProgram::Bind");

    LOG_TRACE("glUseProgram");
    glUseProgram(m_ShaderProgramHandle);
}

void OGLComputeShaderProgram::Clean()
{
    FN("OGLComputeShaderProgram::Clean");

    for (auto it = m_dirtyShaderResources.begin(); it != m_dirtyShaderResources.end(); it++)
    {
        (*it)->Clean();
    }
    m_dirtyShaderResources.clear();
}

bool OGLComputeShaderProgram::IsReady()
{
    FN("OGLComputeShaderProgram::IsReady");

    return m_ready;
}

// =====================================================================
// PRIVATE FUNCTIONS
// =====================================================================

ShaderVertexAttributeMap OGLComputeShaderProgram::FindVertexAttributes()
{
    FN("OGLComputeShaderProgram::FindVertexAttributes");

    int numberOfAttributes;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_ATTRIBUTES, &numberOfAttributes);

    int attributeNameMaxLength;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeNameMaxLength);

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
        glGetActiveAttrib(m_ShaderProgramHandle, i, attributeNameMaxLength, &attributeNameLength, &attributeLength, &attributeType, attributeName);

        if (strncmp(attributeName, "gl_", strlen("gl_")) == 0)
        {
            //
            // Names starting with the reserved prefix of "gl_" have a location
            // of -1.
            continue;
        }
        LOG_TRACE("glGetAttribLocation");
        int attributeLocation = glGetAttribLocation(m_ShaderProgramHandle, attributeName);

        LOG_TRACE("\t\tName: %s, \tLocation: %d", attributeName, attributeLocation);
        vertexAttributes.insert(std::make_pair(attributeLocation, ShaderVertexAttributePtr(new ShaderVertexAttribute(attributeLocation, std::string(attributeName), OGLTypeConverter::ToActiveAttribType(attributeType), attributeLength))));
    }
    return vertexAttributes;
}

ShaderResourceMap OGLComputeShaderProgram::FindShaderResources()
{
    FN("OGLComputeShaderProgram::FindShaderResources");

    int numberOfShaderResources;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_UNIFORMS, &numberOfShaderResources);

    int uniformNameMaxLength;
    LOG_TRACE("glGetProgramiv");
    glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameMaxLength);

    ShaderResourceMap uniforms;
    if (numberOfShaderResources > 0)
        LOG_TRACE("\tShaderResources:");

    for (size_t i = 0; i < (uint32_t)numberOfShaderResources; ++i)
    {
        int uniformNameLength;
        int uniformSize;
        GLenum uniformType;
        GLchar *uniformName = new GLchar[uniformNameMaxLength];

        LOG_TRACE("glGetActiveUniform");
        glGetActiveUniform(m_ShaderProgramHandle, i, uniformNameMaxLength, &uniformNameLength, &uniformSize, &uniformType, uniformName);

        if (strncmp(uniformName, "gl_", strlen("gl_")) == 0)
        {
            //
            // Names starting with the reserved prefix of "gl_" have a location
            // of -1.
            //
            continue;
        }

        //
        // Skip uniforms in a named block
        //
        int uniformBlockIndex;
        uint32_t index = (uint32_t)i;
        LOG_TRACE("glGetActiveUniformsiv");
        glGetActiveUniformsiv(m_ShaderProgramHandle, 1, &index, GL_UNIFORM_BLOCK_INDEX, &uniformBlockIndex);

        if (uniformBlockIndex != -1)
        {
            continue;
        }

        LOG_TRACE("glGetUniformLocation");
        int uniformLocation = glGetUniformLocation(m_ShaderProgramHandle, uniformName);

        LOG_TRACE("\t\tName: %s, \tLocation: %d, \tArraySize: %d", uniformName, uniformLocation, uniformSize);

        LOG_FATAL("Not Yet Implemented");
    }
    return uniforms;
}

std::unordered_map<std::string, ShaderUniformBufferUnit> OGLComputeShaderProgram::FindUniformBuffers()
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

std::unordered_map<std::string, ShaderStorageBufferUnit> OGLComputeShaderProgram::FindStorageBuffers()
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

} // namespace bow
