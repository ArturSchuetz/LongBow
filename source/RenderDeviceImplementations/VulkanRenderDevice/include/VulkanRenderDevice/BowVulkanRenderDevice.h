#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/IBowRenderDevice.h>

namespace bow
{

class VulkanRenderDevice : public IRenderDevice
{
  public:
    VulkanRenderDevice();
    ~VulkanRenderDevice();

    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================

    bool Initialize(int deviceHandle = -1);
    void VRelease() override;

    GraphicsWindowPtr VCreateWindow(int width, int height, const std::string &title, WindowType type) override;

    ///=========================================================================
    //// SHADER STUFF:
    ///=========================================================================

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
    VertexBufferPtr VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes, bool useForRayTracing = false) override;
    IndexBufferPtr VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes, bool useForRayTracing = false) override;
    WritePixelBufferPtr VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) override;
    UniformBufferPtr VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data) override;
    StorageBufferPtr VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data) override;

    Texture2DPtr VCreateTexture2D(Texture2DDescription description) override;
    Texture2DPtr VCreateTexture2D(ImagePtr image) override;

    TextureSamplerPtr VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy) override;

    // ==================== RayTracing ====================

    std::unique_ptr<IRayTracingShaderProgram> VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                             const std::string &intersectionShaderSource, const std::string &callableShaderSource) override;

    std::unique_ptr<IBottomLevelAccelerationStructure> VCreateBottomLevelAccelerationStructure(MeshPtr mesh) override;

    std::unique_ptr<ITopLevelAccelerationStructure> VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures) override;

    ///=========================================================================
    //// VULKAN INTERNAL STUFF:
    ///=========================================================================

    VulkanInstancePtr GetInstance();
    std::vector<VulkanPhysicalDevicePtr> GetAvailablePhysicalDevices();
    VulkanPhysicalDevicePtr GetSelectedPhysicalDevice();
    VulkanLogicalDevicePtr GetLogicalDevice();

  private:
    // you shall not copy!
    VulkanRenderDevice(const VulkanRenderDevice &) = delete;
    VulkanRenderDevice &operator=(const VulkanRenderDevice &) = delete;

    VulkanPhysicalDevicePtr SelectPhysicalDevice(VulkanGraphicsWindowPtr graphicsWindow);

    std::vector<VulkanPhysicalDevicePtr> m_physicalDevices;
    VulkanPhysicalDevicePtr m_selectedPhysicalDevice;
    VulkanLogicalDevicePtr m_logicalDevice;

    uint32_t m_graphicsQueueFamilyIdx;
    uint32_t m_presentQueueFamilyIdx;
    uint32_t m_computeQueueFamilyIndex;
    uint32_t m_transferQueueFamilyIndex;

    std::string m_guid;
};
} // namespace bow
