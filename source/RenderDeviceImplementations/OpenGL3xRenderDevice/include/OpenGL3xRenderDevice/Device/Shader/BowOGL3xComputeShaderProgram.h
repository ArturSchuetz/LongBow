#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xStorageBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xUniformBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/IBowComputeShaderProgram.h>

#include <CoreSystems/DesignPattern/IBowCleanableObserver.h>

namespace bow
{

typedef std::shared_ptr<class OGLComputeShaderProgramName> OGLComputeShaderProgramNamePtr;
typedef std::shared_ptr<class OGLShaderObject> OGLShaderObjectPtr;
typedef std::shared_ptr<class OGLFragmentOutputs> OGLFragmentOutputsPtr;

enum class ShaderResourceType : char;
typedef std::shared_ptr<class OGLShaderResource> OGLShaderResourcePtr;
typedef std::unordered_map<std::string, OGLShaderResourcePtr> ShaderResourceMap;

class OGLComputeShaderProgram : public IComputeShaderProgram, public ICleanableObserver
{
    enum class PrimitiveDatatype : char
    {
        Int = 0,
        Float,
        Uint
    };

  public:
    OGLComputeShaderProgram(const std::string &computeShaderSource);
    ~OGLComputeShaderProgram();

    // ===========================================
    // IShaderProgram Functions
    // ===========================================

    ShaderResourceBindingsPtr VCreateComputeResourceBindingObjects();
    void VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    // ===========================================
    // ICleanableObserver Functions
    // ===========================================

    void NotifyDirty(ICleanable *value);

    // ===========================================

    std::string GetLog();
    uint32_t GetProgram();

    void Bind();
    void Clean();

    bool IsReady();

  private:
    // you shall not copy
    OGLComputeShaderProgram(OGLComputeShaderProgram &) = delete;
    OGLComputeShaderProgram &operator=(const OGLComputeShaderProgram &) = delete;

    ShaderVertexAttributeMap FindVertexAttributes();
    ShaderResourceMap FindShaderResources();
    std::unordered_map<std::string, ShaderUniformBufferUnit> FindUniformBuffers();
    std::unordered_map<std::string, ShaderStorageBufferUnit> FindStorageBuffers();

    OGLShaderObjectPtr m_computeShader;

    ShaderVertexAttributeMap m_shaderVertexAttributes;
    OGLFragmentOutputsPtr m_fragmentOutputs;

    ShaderResourceMap m_uniforms;
    std::unordered_map<std::string, ShaderUniformBufferUnit> m_uniformBuffers;
    std::unordered_map<std::string, ShaderStorageBufferUnit> m_storageBuffers;

    std::list<ICleanable *> m_dirtyShaderResources;
    bool m_ready;

    uint32_t m_ShaderProgramHandle;
};

typedef std::shared_ptr<OGLComputeShaderProgram> OGLComputeShaderProgramPtr;

} // namespace bow
