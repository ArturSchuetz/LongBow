#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xStorageBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xUniformBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/IBowShaderProgram.h>

namespace bow
{

typedef std::shared_ptr<class OGLShaderProgramName> OGLShaderProgramNamePtr;
typedef std::shared_ptr<class OGLShaderObject> OGLShaderObjectPtr;
typedef std::shared_ptr<class OGLFragmentOutputs> OGLFragmentOutputsPtr;

enum class ShaderResourceType : char;
typedef std::shared_ptr<class OGLShaderResource> OGLShaderResourcePtr;
typedef std::unordered_map<std::string, OGLShaderResourcePtr> ShaderResourceMap;

class OGLShaderProgram : public IShaderProgram
{
    enum class PrimitiveDatatype : char
    {
        Int = 0,
        Float,
        Uint
    };

  public:
    OGLShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource);
    ~OGLShaderProgram();

    // ===========================================
    // IShaderProgram Functions
    // ===========================================

    ShaderVertexAttributePtr VGetVertexAttribute(std::string name) override;
    ShaderVertexAttributeMap VGetVertexAttributes() override;
    int VGetFragmentOutputLocation(std::string name) override;

    ShaderResourceBindingsPtr VCreateResourceBindingObjects() override;

    void VSetPushConstants(const char *name, const void *data, size_t offset, size_t size) override;
    void VSetPushConstants(ShaderStage shaderStage, const void *data, size_t offset, size_t size) override;

    // ===========================================

    std::string GetLog();
    uint32_t GetProgram();

    void Bind();

    bool IsReady();

  private:
    // you shall not copy
    OGLShaderProgram(OGLShaderProgram &) = delete;
    OGLShaderProgram &operator=(const OGLShaderProgram &) = delete;

    ShaderVertexAttributeMap FindVertexAttributes(uint32_t Program);
    ShaderResourceMap FindShaderResources(uint32_t program);
    std::unordered_map<std::string, ShaderUniformBufferUnit> FindUniformBuffers();
    std::unordered_map<std::string, ShaderStorageBufferUnit> FindStorageBuffers();

    OGLShaderObjectPtr m_computeShader;
    OGLShaderObjectPtr m_vertexShader;
    OGLShaderObjectPtr m_geometryShader;
    OGLShaderObjectPtr m_fragmentShader;

    ShaderVertexAttributeMap m_shaderVertexAttributes;
    OGLFragmentOutputsPtr m_fragmentOutputs;

    ShaderResourceMap m_shaderResources;
    std::unordered_map<std::string, ShaderUniformBufferUnit> m_uniformBuffers;
    std::unordered_map<std::string, ShaderStorageBufferUnit> m_storageBuffers;

    bool m_ready;

    uint32_t m_ShaderProgramHandle;
};

typedef std::shared_ptr<OGLShaderProgram> OGLShaderProgramPtr;

} // namespace bow
