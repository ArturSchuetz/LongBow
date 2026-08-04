#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanDeviceMemory
{
  public:
    VulkanDeviceMemory(VulkanLogicalDevice *logicalDevice, VulkanBuffer *buffer);
    ~VulkanDeviceMemory();

    bool AllocateMemory(VkMemoryPropertyFlags properties);

    VkDeviceMemory GetHandle();

  private:
    // you shall not copy!
    VulkanDeviceMemory(const VulkanDeviceMemory &) = delete;
    VulkanDeviceMemory &operator=(const VulkanDeviceMemory &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VulkanBuffer *m_buffer;
    VkDeviceMemory m_deviceMemory;

    std::string m_guid;
};

} // namespace bow
