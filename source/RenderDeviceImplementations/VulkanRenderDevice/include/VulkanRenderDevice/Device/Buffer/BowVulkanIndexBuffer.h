#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/Device/Buffer/BowVulkanBufferBase.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowIndexBuffer.h>

namespace bow
{

class VulkanIndexBuffer : public IIndexBuffer
{
  public:
    VulkanIndexBuffer(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes, bool enableShaderDeviceAdressBit);
    ~VulkanIndexBuffer();

    bool Initialize();

    void Bind(VkCommandBuffer commandBuffer);

    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;

    VkDeviceAddress GetBufferDeviceAddress();

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    VkBuffer GetHandle() const;
    uint32_t GetIndexCount() const;

    IndexBufferDatatype GetDatatype() const;

  private:
    // you shall not copy!
    VulkanIndexBuffer(const VulkanIndexBuffer &) = delete;
    VulkanIndexBuffer &operator=(const VulkanIndexBuffer &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    IndexBufferDatatype m_datatype;
    bool m_enableShaderDeviceAdressBit;

    VulkanBufferBase m_buffer;

    std::string m_guid;
};

} // namespace bow