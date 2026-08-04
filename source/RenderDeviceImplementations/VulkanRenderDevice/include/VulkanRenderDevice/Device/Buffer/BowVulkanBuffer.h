#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanBuffer
{
  public:
    VulkanBuffer(VulkanLogicalDevice *logicalDevice);
    ~VulkanBuffer();

    bool Initialize(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    VulkanDeviceMemoryPtr GetDeviceMemory();

    VkDeviceAddress GetBufferDeviceAddress();

    bool UseShaderDeviceAdressBit();
    VkBuffer GetHandle();

  private:
    // you shall not copy!
    VulkanBuffer(const VulkanBuffer &) = delete;
    VulkanBuffer &operator=(const VulkanBuffer &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    bool m_useShaderDeviceAdressBit;

    VkBuffer m_buffer;
    VulkanDeviceMemoryPtr m_deviceMemory;

    std::string m_guid;
};

} // namespace bow
