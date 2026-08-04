#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>

namespace bow
{

class VulkanLogicalDevice
{
  public:
    VulkanLogicalDevice();
    ~VulkanLogicalDevice();

    bool Initialize(VulkanPhysicalDevice *physicalDevice, uint32_t graphicsFamilyIdx, uint32_t presentFamilyIdx, uint32_t computeFamilyIndex, uint32_t transferFamilyIndex);
    void Release();

    VkDevice GetHandle();
    VulkanPhysicalDevice *GetParentPhysicalDevice();

    VulkanShaderProgramPtr CreateComputeShaderProgram(const std::string &computeShaderSource);
    VulkanShaderProgramPtr CreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                               const std::string &TessEvalShaderSource);
    std::unique_ptr<VulkanRayTracingShaderProgram> CreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                                 const std::string &intersectionShaderSource, const std::string &callableShaderSource);

    VulkanSwapchainPtr CreateSwapchain(VulkanRenderSurface *surface, int width, int height);

    VulkanBufferPtr CreateBuffer(VkBufferUsageFlags usageFlags, int32_t sizeInBytes, VkMemoryPropertyFlags properties = 0);
    VulkanVertexBufferPtr CreateVertexBuffer(BufferHint usageHint, int32_t sizeInBytes, bool useShaderDeviceAdressBit);
    VulkanIndexBufferPtr CreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes, bool useShaderDeviceAdressBit);
    VulkanUniformBufferPtr CreateUniformBuffer(BufferHint usageHint, int64_t sizeInBytes);
    VulkanStorageBufferPtr CreateStorageBuffer(BufferHint usageHint, int64_t sizeInBytes);

    uint32_t GetPresentQueueFamilyIndex() const;
    uint32_t GetGraphicsQueueFamilyIndex() const;
    uint32_t GetComputeQueueFamilyIndex() const;
    uint32_t GetTransferQueueFamilyIndex() const;

    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;
    VkQueue GetComputeQueue() const;
    VkQueue GetTransferQueue() const;

    VulkanCommandPoolPtr GetGraphicsCommandPool() const;
    VulkanCommandPoolPtr GetPresentCommandPool() const;
    VulkanCommandPoolPtr GetComputeCommandPool() const;
    VulkanCommandPoolPtr GetTransferCommandPool() const;

    VulkanCommandBufferPtr CreateGraphicsCommandBuffer();
    VulkanCommandBufferPtr CreatePresentCommandBuffer();
    VulkanCommandBufferPtr CreateComputeCommandBuffer();
    VulkanCommandBufferPtr CreateTransferCommandBuffer();

    VulkanSingleUseCommandBufferPtr CreateSingleUseGraphicsCommandBuffer();
    VulkanSingleUseCommandBufferPtr CreateSingleUsePresentCommandBuffer();
    VulkanSingleUseCommandBufferPtr CreateSingleUseComputeCommandBuffer();
    VulkanSingleUseCommandBufferPtr CreateSingleUseTransferCommandBuffer();

    VulkanSemaphorePtr CreateSemaphoreA();
    VulkanFencePtr CreateFence();

  private:
    // you shall not copy!
    VulkanLogicalDevice(const VulkanLogicalDevice &) = delete;
    VulkanLogicalDevice &operator=(const VulkanLogicalDevice &) = delete;

    VulkanPhysicalDevice *m_physicalDevice;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkQueue m_computeQueue;
    VkQueue m_transferQueue;
    uint32_t m_graphicsQueueFamilyIndex;
    uint32_t m_presentQueueFamilyIndex;
    uint32_t m_computeQueueFamilyIndex;
    uint32_t m_transferQueueFamilyIndex;
    VulkanCommandPoolPtr m_graphicsCommandPool;
    VulkanCommandPoolPtr m_presentCommandPool;
    VulkanCommandPoolPtr m_computeCommandPool;
    VulkanCommandPoolPtr m_transferCommandPool;

    std::string m_guid;
};
} // namespace bow