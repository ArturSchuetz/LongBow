#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

typedef enum class VulkanCommandBufferState
{
    Invalid,
    Ready,
    Recording,
    RecordingEnded,
    Submitted,
    NotAllocated
} VulkanCommandBufferState;

class VulkanCommandBuffer
{
  public:
    VulkanCommandBuffer();
    ~VulkanCommandBuffer();

    bool Allocate(VulkanLogicalDevice *logicalDevice, VulkanCommandPool *commandPool, uint32_t count = 1, bool isPrimary = true);
    void Release();

    VkCommandBuffer GetHandle(uint32_t index = 0) const;

    uint32_t GetCount() const;
    VulkanCommandBufferState GetState(uint32_t index = 0) const;

    bool Begin(uint32_t index = 0, bool isSingleUse = false, bool isRenderpassContinue = false, bool isSimultaniousUse = false);
    bool End(uint32_t index = 0);

    bool Submit(VkQueue queue, uint32_t index = 0, VkSemaphore waitForSemaphore = nullptr, VkSemaphore signalSemaphore = nullptr);
    bool Submit(VkQueue queue, uint32_t index, VkSubmitInfo submitInfo);

    bool WaitForQueueIdle(VkQueue queue, uint32_t index = 0);
    bool Reset(uint32_t index = 0);

    std::vector<VkSemaphore> GetWaitSemaphores();
    void ClearSemaphores();

  private:
    // you shall not copy!
    VulkanCommandBuffer(const VulkanCommandBuffer &) = delete;
    VulkanCommandBuffer &operator=(const VulkanCommandBuffer &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VulkanCommandPool *m_commandPool;

    bool m_isPrimary;
    uint32_t m_count;
    std::vector<VulkanCommandBufferState> m_commandBufferStates;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_waitForSemaphores;

    std::string m_guid;
};

} // namespace bow