#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/Device/Buffer/BowVulkanBufferBase.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowVertexBuffer.h>

namespace bow
{

class VulkanVertexBuffer : public IVertexBuffer
{
  public:
    VulkanVertexBuffer(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, int64_t sizeInBytes, bool enableShaderDeviceAdressBit);
    ~VulkanVertexBuffer();

    bool Initialize();

    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;

    VkDeviceAddress GetBufferDeviceAddress();

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    VkBuffer GetHandle() const;

  private:
    // you shall not copy!
    VulkanVertexBuffer(const VulkanVertexBuffer &) = delete;
    VulkanVertexBuffer &operator=(const VulkanVertexBuffer &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    bool m_enableShaderDeviceAdressBit;

    VulkanBufferBase m_buffer;

    std::string m_guid;
};

} // namespace bow
