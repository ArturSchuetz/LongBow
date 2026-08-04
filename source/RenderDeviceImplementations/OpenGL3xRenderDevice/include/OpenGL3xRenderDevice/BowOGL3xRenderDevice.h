#pragma once
#include <RenderDevice/IBowRenderDevice.h>

namespace bow
{

typedef std::shared_ptr<class OGLGraphicsWindow> OGLGraphicsWindowPtr;
typedef std::unordered_map<uint32_t, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;

class OGLRenderDevice : public IRenderDevice
{
  public:
    OGLRenderDevice();
    ~OGLRenderDevice();

    //=========================================================================
    // INIT/RELEASE STUFF:
    //=========================================================================

    bool Initialize();
    void VRelease() override;

    GraphicsWindowPtr VCreateWindow(int width, int height, const std::string &title, WindowType type) override;

    //=========================================================================
    // SHADER STUFF:
    //=========================================================================

    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragementShaderFilename) override;
    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &GeometryShaderFilename, const std::string &FragementShaderFilename) override;
    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &TessControlShaderFilename, const std::string &TessEvalShaderFilename) override;
    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &GeometryShaderFilename, const std::string &TessControlShaderFilename,
                                                  const std::string &TessEvalShaderFilename) override;

    ComputeShaderProgramPtr VCreateComputeShaderProgram(const std::string &computeShaderSource) override;

    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragementShaderSource) override;

    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &GeometryShaderSource, const std::string &FragementShaderSource) override;

    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource) override;

    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                          const std::string &TessEvalShaderSource) override;

    MeshBufferPtr VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) override;
    VertexBufferPtr VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes) override;
    IndexBufferPtr VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes) override;
    UniformBufferPtr VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data) override;
    StorageBufferPtr VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data) override;

    WritePixelBufferPtr VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) override;

    Texture2DPtr VCreateTexture2D(Texture2DDescription description) override;
    Texture2DPtr VCreateTexture2D(ImagePtr image) override;

    TextureSamplerPtr VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy) override;

    // ==================== RayTracing ====================

    std::unique_ptr<IRayTracingShaderProgram> VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                             const std::string &intersectionShaderSource, const std::string &callableShaderSource) override;

    std::unique_ptr<IBottomLevelAccelerationStructure> VCreateBottomLevelAccelerationStructure(VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings) override;
    std::unique_ptr<IBottomLevelAccelerationStructure> VCreateBottomLevelAccelerationStructure(uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings) override;

    std::unique_ptr<ITopLevelAccelerationStructure> VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures) override;

  private:
    // you shall not copy
    OGLRenderDevice(OGLRenderDevice &) = delete;
    OGLRenderDevice &operator=(const OGLRenderDevice &) = delete;

    int m_maximumNumberOfVertexAttributes;
    int m_numberOfTextureUnits;
    int m_maximumNumberOfColorAttachments;

    // std::unordered_map<uint32_t, Texture2DPtr> textures;
};

typedef std::shared_ptr<OGLRenderDevice> OGLRenderDevicePtr;
typedef std::unordered_map<uint32_t, OGLRenderDevice> OGLRenderDeviceMap;

} // namespace bow
