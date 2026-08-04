#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowUniformBuffer.h>

namespace bow
{

class VulkanUniformBuffer : public IUniformBuffer
{
  public:
    VulkanUniformBuffer(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, int64_t sizeInBytes);
    ~VulkanUniformBuffer();

    bool Initialize();

    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;
    std::shared_ptr<void> VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) override;

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    VkBuffer GetHandle() const;
    VulkanDeviceMemoryPtr GetDeviceMemory() const;

  private:
    // you shall not copy!
    VulkanUniformBuffer(const VulkanUniformBuffer &) = delete;
    VulkanUniformBuffer &operator=(const VulkanUniformBuffer &) = delete;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VulkanLogicalDevice *m_logicalDevice;
    int64_t m_sizeInBytes;
    BufferHint m_usageHint;

    VulkanBufferPtr m_buffer;
    VulkanDeviceMemoryPtr m_deviceMemory;

    std::string m_guid;
};

} // namespace bow
