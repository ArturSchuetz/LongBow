#pragma once
#include <VulkanThinDevice/BowVulkanThinCommon.h>

#include <vector>

namespace bow
{

class VulkanThinDevice;

//! A queue, and the only place work is submitted.
class VulkanThinQueue : public IThinQueue
{
  public:
    VulkanThinQueue(VulkanThinDevice *device, VkQueue queue, ThinQueueType type);
    ~VulkanThinQueue();

    ThinQueueType VGetType() const override { return m_type; }

    void VSubmit(const std::vector<ThinCommandListPtr> &commandLists, const ThinFencePtr &waitFence, uint64_t waitValue, const ThinFencePtr &signalFence, uint64_t signalValue) override;
    void VWaitIdle() override;

    VkQueue GetHandle() const { return m_queue; }

  private:
    VulkanThinDevice *m_device;
    VkQueue m_queue;
    ThinQueueType m_type;
};

//! A timeline semaphore, which is the only fence this backend has.
class VulkanThinFence : public IThinFence
{
  public:
    VulkanThinFence(VulkanThinDevice *device, uint64_t initialValue);
    ~VulkanThinFence();

    uint64_t VGetCompletedValue() const override;
    void VWait(uint64_t value) override;
    void VSignal(uint64_t value) override;

    VkSemaphore GetHandle() const { return m_semaphore; }

  private:
    VulkanThinDevice *m_device;
    VkSemaphore m_semaphore;
};

class VulkanThinCommandList : public IThinCommandList
{
  public:
    VulkanThinCommandList(VulkanThinDevice *device, VkCommandBuffer commandBuffer);
    ~VulkanThinCommandList();

    void VBegin() override;
    void VEnd() override;

    void VBeginRendering(const ThinRenderingInfo &renderingInfo) override;
    void VEndRendering() override;

    void VSetViewport(const ThinViewport &viewport) override;
    void VSetScissor(const ThinScissor &scissor) override;

    void VBindPipeline(const ThinPipelinePtr &pipeline) override;
    void VBindDescriptorSet(uint32_t setIndex, const ThinDescriptorSetPtr &descriptorSet) override;
    void VSetRootConstants(ThinShaderStage stages, uint32_t offsetInBytes, uint32_t sizeInBytes, const void *data) override;

    void VBindVertexBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes) override;
    void VBindIndexBuffer(const ThinBufferPtr &buffer, ThinFormat indexFormat, uint64_t offsetInBytes) override;

    void VDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    void VDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
    void VDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
    void VTraceRays(uint32_t width, uint32_t height, uint32_t depth) override;

    void VCopyBuffer(const ThinBufferPtr &source, const ThinBufferPtr &destination, uint64_t sourceOffset, uint64_t destinationOffset, uint64_t sizeInBytes) override;
    void VCopyBufferToTexture(const ThinBufferPtr &source, const ThinTexturePtr &destination, uint32_t mipLevel) override;

    void VBarrier(const std::vector<ThinBarrier> &barriers) override;

    void VBeginDebugLabel(const char *name) override;
    void VEndDebugLabel() override;

    VkCommandBuffer GetHandle() const { return m_commandBuffer; }

  private:
    VulkanThinDevice *m_device;
    VkCommandBuffer m_commandBuffer;

    //! Layout the pipeline currently bound was built with, for descriptor binding.
    VkPipelineLayout m_boundLayout;
    VkPipelineBindPoint m_boundBindPoint;
};

class VulkanThinCommandPool : public IThinCommandPool
{
  public:
    VulkanThinCommandPool(VulkanThinDevice *device, uint32_t queueFamily);
    ~VulkanThinCommandPool();

    ThinCommandListPtr VAllocate() override;
    void VReset() override;

  private:
    VulkanThinDevice *m_device;
    VkCommandPool m_pool;
};

class VulkanThinBuffer : public IThinBuffer
{
  public:
    VulkanThinBuffer(VulkanThinDevice *device, const ThinBufferDescription &description);
    ~VulkanThinBuffer();

    const ThinBufferDescription &VGetDescription() const override { return m_description; }

    void *VMap() override;
    void VUnmap() override;
    uint64_t VGetDeviceAddress() const override;

    VkBuffer GetHandle() const { return m_buffer; }
    bool IsReady() const { return m_buffer != VK_NULL_HANDLE; }

  private:
    VulkanThinDevice *m_device;
    ThinBufferDescription m_description;

    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    void *m_mapped;
};

class VulkanThinTexture : public IThinTexture
{
  public:
    //! Creates a texture and its memory.
    VulkanThinTexture(VulkanThinDevice *device, const ThinTextureDescription &description);
    //! Wraps an image the swapchain owns; the image is not destroyed here.
    VulkanThinTexture(VulkanThinDevice *device, VkImage image, const ThinTextureDescription &description);
    ~VulkanThinTexture();

    const ThinTextureDescription &VGetDescription() const override { return m_description; }

    VkImage GetImage() const { return m_image; }
    VkImageView GetView() const { return m_view; }
    bool IsReady() const { return m_image != VK_NULL_HANDLE; }

  private:
    bool CreateView();

    VulkanThinDevice *m_device;
    ThinTextureDescription m_description;

    VkImage m_image;
    VkImageView m_view;
    VkDeviceMemory m_memory;
    bool m_ownsImage;
};

class VulkanThinSampler : public IThinSampler
{
  public:
    VulkanThinSampler(VulkanThinDevice *device, const ThinSamplerDescription &description);
    ~VulkanThinSampler();

    VkSampler GetHandle() const { return m_sampler; }

  private:
    VulkanThinDevice *m_device;
    VkSampler m_sampler;
};

class VulkanThinShaderModule : public IThinShaderModule
{
  public:
    VulkanThinShaderModule(VulkanThinDevice *device, const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char *entryPoint);
    ~VulkanThinShaderModule();

    ThinShaderStage VGetStage() const override { return m_stage; }

    VkShaderModule GetHandle() const { return m_module; }
    const std::string &GetEntryPoint() const { return m_entryPoint; }

  private:
    VulkanThinDevice *m_device;
    VkShaderModule m_module;
    ThinShaderStage m_stage;
    std::string m_entryPoint;
};

class VulkanThinPipelineLayout : public IThinPipelineLayout
{
  public:
    VulkanThinPipelineLayout(VulkanThinDevice *device, const ThinPipelineLayoutDescription &description);
    ~VulkanThinPipelineLayout();

    const ThinPipelineLayoutDescription &VGetDescription() const override { return m_description; }

    VkPipelineLayout GetHandle() const { return m_layout; }
    const std::vector<VkDescriptorSetLayout> &GetSetLayouts() const { return m_setLayouts; }

  private:
    VulkanThinDevice *m_device;
    ThinPipelineLayoutDescription m_description;

    std::vector<VkDescriptorSetLayout> m_setLayouts;
    VkPipelineLayout m_layout;
};

class VulkanThinPipeline : public IThinPipeline
{
  public:
    VulkanThinPipeline(VulkanThinDevice *device, VkPipeline pipeline, VkPipelineBindPoint bindPoint, const ThinPipelineLayoutPtr &layout);
    ~VulkanThinPipeline();

    ThinPipelineLayoutPtr VGetLayout() const override { return m_layout; }

    VkPipeline GetHandle() const { return m_pipeline; }
    VkPipelineBindPoint GetBindPoint() const { return m_bindPoint; }

  private:
    VulkanThinDevice *m_device;
    VkPipeline m_pipeline;
    VkPipelineBindPoint m_bindPoint;
    ThinPipelineLayoutPtr m_layout;
};

class VulkanThinDescriptorSet : public IThinDescriptorSet
{
  public:
    VulkanThinDescriptorSet(VulkanThinDevice *device, VkDescriptorSet set);
    ~VulkanThinDescriptorSet();

    void VSetBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes, uint64_t sizeInBytes) override;
    void VSetTexture(uint32_t binding, const ThinTexturePtr &texture) override;
    void VSetStorageTexture(uint32_t binding, const ThinTexturePtr &texture) override;
    void VSetSampler(uint32_t binding, const ThinSamplerPtr &sampler) override;
    void VSetAccelerationStructure(uint32_t binding, const ThinAccelerationStructurePtr &accelerationStructure) override;
    void VUpdate() override;

    VkDescriptorSet GetHandle() const { return m_set; }

  private:
    //! One pending write, kept until VUpdate flushes them together.
    struct PendingWrite
    {
        uint32_t binding;
        VkDescriptorType type;
        VkDescriptorBufferInfo bufferInfo;
        VkDescriptorImageInfo imageInfo;
    };

    VulkanThinDevice *m_device;
    VkDescriptorSet m_set;
    std::vector<PendingWrite> m_pending;
};

class VulkanThinDescriptorPool : public IThinDescriptorPool
{
  public:
    VulkanThinDescriptorPool(VulkanThinDevice *device, uint32_t maxSets);
    ~VulkanThinDescriptorPool();

    ThinDescriptorSetPtr VAllocate(const ThinDescriptorSetLayoutDescription &layout) override;
    void VReset() override;

  private:
    VulkanThinDevice *m_device;
    VkDescriptorPool m_pool;
    //! Set layouts created on demand, kept alive for the pool's lifetime.
    std::vector<VkDescriptorSetLayout> m_setLayouts;
};

class VulkanThinSwapchain : public IThinSwapchain
{
  public:
    VulkanThinSwapchain(VulkanThinDevice *device, void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount);
    ~VulkanThinSwapchain();

    uint32_t VAcquireNextImage() override;
    ThinTexturePtr VGetImage(uint32_t index) const override;
    uint32_t VGetImageCount() const override { return (uint32_t)m_images.size(); }
    ThinFormat VGetFormat() const override;

    uint32_t VGetWidth() const override { return m_width; }
    uint32_t VGetHeight() const override { return m_height; }

    void VPresent(bool vsync) override;
    void VResize(uint32_t width, uint32_t height) override;

    bool IsReady() const { return m_swapchain != VK_NULL_HANDLE; }

  private:
    bool CreateSurface(void *nativeWindowHandle);
    bool CreateSwapchain(uint32_t imageCount);
    void DestroySwapchain();

    VulkanThinDevice *m_device;

    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapchain;
    VkFormat m_format;

    std::vector<ThinTexturePtr> m_images;

    //! Binary semaphores, the one place Vulkan still requires them.
    std::vector<VkSemaphore> m_acquireSemaphores;
    std::vector<VkSemaphore> m_presentSemaphores;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_currentImage;
    uint32_t m_frame;
};

//! An acceleration structure and the address shaders reach it by.
class VulkanThinAccelerationStructure : public IThinAccelerationStructure
{
  public:
    VulkanThinAccelerationStructure(VulkanThinDevice *device, VkAccelerationStructureKHR handle, VkBuffer buffer, VkDeviceMemory memory, uint64_t deviceAddress);
    ~VulkanThinAccelerationStructure();

    uint64_t VGetDeviceAddress() const override { return m_deviceAddress; }

    VkAccelerationStructureKHR GetHandle() const { return m_handle; }

  private:
    VulkanThinDevice *m_device;
    VkAccelerationStructureKHR m_handle;
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    uint64_t m_deviceAddress;
};

} // namespace bow
