#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanCommandPool
{
  public:
    VulkanCommandPool();
    ~VulkanCommandPool();

    bool Initialize(VulkanLogicalDevice *logicalDevice, uint32_t queueFamilyIndex);
    void Release();

    VkCommandPool GetHandle() const;

    VulkanCommandBufferPtr AllocateCommandBuffers(uint32_t count = 1);

  private:
    // you shall not copy!
    VulkanCommandPool(const VulkanCommandPool &) = delete;
    VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VkCommandPool m_commandPool;

    std::string m_guid;
};

} // namespace bow