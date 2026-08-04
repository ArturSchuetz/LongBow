#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanSemaphore
{
  public:
    VulkanSemaphore();
    ~VulkanSemaphore();

    bool Initialize(VulkanLogicalDevice *logicalDevice);
    void Release();

    VkSemaphore GetHandle() const { return m_semaphore; }

  private:
    // you shall not copy!
    VulkanSemaphore(const VulkanSemaphore &) = delete;
    VulkanSemaphore &operator=(const VulkanSemaphore &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VkSemaphore m_semaphore;

    std::string m_guid;
};

} // namespace bow