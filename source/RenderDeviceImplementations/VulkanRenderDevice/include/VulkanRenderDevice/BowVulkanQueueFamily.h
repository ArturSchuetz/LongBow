#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanQueueFamily
{
  public:
    VulkanQueueFamily(uint32_t familyIndex, const VkQueueFamilyProperties &queue_family_properties);
    ~VulkanQueueFamily();

    uint32_t GetFamilyIndex();
    uint32_t GetQueueCount();

    bool IsGraphicsQueue();
    bool IsComputeQueue();
    bool IsTransferQueue();
    bool IsSparseBindingQueue();
    bool IsProtectedQueue();
    bool IsPresentQueue(VkPhysicalDevice device, uint32_t familyIndex, VkSurfaceKHR surface);

  private:
    // you shall not copy!
    VulkanQueueFamily(const VulkanQueueFamily &) = delete;
    VulkanQueueFamily &operator=(const VulkanQueueFamily &) = delete;

    uint32_t m_familyIndex;
    VkQueueFamilyProperties m_properties;

    std::string m_guid;
};
} // namespace bow