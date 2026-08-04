#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

enum class VulkanCommandBufferState;

class VulkanSingleUseCommandBuffer
{
  public:
    VulkanSingleUseCommandBuffer();
    ~VulkanSingleUseCommandBuffer();

    bool AllocateAndBegin(VulkanLogicalDevice *logicalDevice, VulkanCommandPool *commandPool, VkQueue queue);

    bool EndAndSubmit();
    void Release();

    VkCommandBuffer GetHandle() const;
    VulkanCommandBufferState GetState() const;

  private:
    // you shall not copy!
    VulkanSingleUseCommandBuffer(const VulkanSingleUseCommandBuffer &) = delete;
    VulkanSingleUseCommandBuffer &operator=(const VulkanSingleUseCommandBuffer &) = delete;

    VulkanCommandBufferPtr m_commandBuffer;
    VkQueue m_queue;

    std::string m_guid;
};

} // namespace bow