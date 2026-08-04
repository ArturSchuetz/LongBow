#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

class VulkanBufferBase
{
  public:
    VulkanBufferBase(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, int64_t sizeInBytes);
    ~VulkanBufferBase();

    bool Initialize(VkBufferUsageFlags usage);

    void CopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes);
    std::shared_ptr<void> CopyToSystemMemory(int64_t offsetInBytes, int64_t lengthInBytes);

    VkDeviceAddress GetBufferDeviceAddress();

    int64_t GetSizeInBytes() const;
    BufferHint GetUsageHint() const;

    VkBuffer GetHandle() const;

  private:
    // you shall not copy!
    VulkanBufferBase(const VulkanBufferBase &) = delete;
    VulkanBufferBase &operator=(const VulkanBufferBase &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    int64_t m_sizeInBytes;
    BufferHint m_usageHint;
    bool m_useShaderDeviceAdressBit;

    VulkanBufferPtr m_buffer;

    std::string m_guid;
};

} // namespace bow