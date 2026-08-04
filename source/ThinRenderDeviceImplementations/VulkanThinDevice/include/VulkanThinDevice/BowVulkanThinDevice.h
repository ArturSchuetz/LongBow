#pragma once
#include <VulkanThinDevice/BowVulkanThinCommon.h>

#include <unordered_map>

namespace bow
{

//! Vulkan behind the thin interface.
/*!
    Vulkan 1.3 is the floor, because dynamic rendering and synchronisation2 are
    core there and the interface is built on both. Timeline semaphores came in
    1.2 and are the only fence this uses.
*/
class VulkanThinDevice : public IThinDevice
{
  public:
    VulkanThinDevice();
    ~VulkanThinDevice();

    bool Initialize(uint32_t deviceHandle);
    void VRelease() override;

    const ThinDeviceCapabilities &VGetCapabilities() const override { return m_capabilities; }

    ThinQueuePtr VGetQueue(ThinQueueType type) override;

    ThinSwapchainPtr VCreateSwapchain(void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount) override;

    ThinCommandPoolPtr VCreateCommandPool(ThinQueueType type) override;
    ThinFencePtr VCreateFence(uint64_t initialValue) override;

    ThinBufferPtr VCreateBuffer(const ThinBufferDescription &description) override;
    ThinTexturePtr VCreateTexture(const ThinTextureDescription &description) override;
    ThinSamplerPtr VCreateSampler(const ThinSamplerDescription &description) override;

    ThinShaderModulePtr VCreateShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char *entryPoint) override;

    ThinPipelineLayoutPtr VCreatePipelineLayout(const ThinPipelineLayoutDescription &description) override;
    ThinPipelinePtr VCreateGraphicsPipeline(const ThinGraphicsPipelineDescription &description) override;
    ThinPipelinePtr VCreateComputePipeline(const ThinComputePipelineDescription &description) override;
    ThinPipelinePtr VCreateRayTracingPipeline(const ThinRayTracingPipelineDescription &description) override;

    ThinDescriptorPoolPtr VCreateDescriptorPool(uint32_t maxSets) override;

    ThinAccelerationStructurePtr VCreateBottomLevelAccelerationStructure(const ThinBufferPtr &vertexBuffer, uint32_t vertexCount, uint32_t vertexStrideInBytes, const ThinBufferPtr &indexBuffer, uint32_t indexCount) override;
    ThinAccelerationStructurePtr VCreateTopLevelAccelerationStructure(const std::vector<ThinAccelerationStructurePtr> &bottomLevel) override;

    void VWaitIdle() override;

    VkInstance GetInstance() const { return m_instance; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    VkDevice GetHandle() const { return m_device; }
    uint32_t GetQueueFamily(ThinQueueType type) const;

    //! Picks a memory type satisfying the given requirements.
    /*!
        \param typeBits  Mask of acceptable types, from the resource.
        \param properties Properties the memory has to have.
        \return Index of a suitable type, or UINT32_MAX when there is none.
    */
    uint32_t FindMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

    //! Binary semaphores the next submit has to wait on and signal.
    /*!
        Swapchain acquire and present are the one place Vulkan still requires
        binary semaphores, and the thin interface deliberately does not expose
        them. The swapchain leaves the pair for the current image here, the
        next submit consumes it, and present waits on the signalled one. That
        keeps the whole mechanism inside the backend.
    */
    struct PendingPresentSync
    {
        VkSemaphore waitOnAcquire = VK_NULL_HANDLE;
        VkSemaphore signalForPresent = VK_NULL_HANDLE;
    };

    void SetPendingPresentSync(const PendingPresentSync &sync) { m_pendingPresentSync = sync; }
    PendingPresentSync TakePendingPresentSync();

  private:
    VulkanThinDevice(const VulkanThinDevice &) = delete;
    VulkanThinDevice &operator=(const VulkanThinDevice &) = delete;

    bool CreateInstance();
    bool PickPhysicalDevice(uint32_t deviceHandle);
    bool CreateLogicalDevice();

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceMemoryProperties m_memoryProperties;
    VkDevice m_device;

    uint32_t m_graphicsFamily;
    uint32_t m_computeFamily;
    uint32_t m_transferFamily;

    std::unordered_map<uint32_t, ThinQueuePtr> m_queues;

    PendingPresentSync m_pendingPresentSync;

    ThinDeviceCapabilities m_capabilities;
    bool m_initialized;
};

} // namespace bow
