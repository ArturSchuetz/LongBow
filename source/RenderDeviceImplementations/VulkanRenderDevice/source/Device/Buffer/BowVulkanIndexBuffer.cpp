#include <VulkanRenderDevice/Device/Buffer/BowVulkanIndexBuffer.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBufferBase.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexAttributeBindingsSizes.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanIndexBuffer::VulkanIndexBuffer(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes, bool enableShaderDeviceAdressBit)
    : m_logicalDevice(logicalDevice), m_datatype(dataType), m_enableShaderDeviceAdressBit(enableShaderDeviceAdressBit), m_buffer(m_logicalDevice, usageHint, sizeInBytes), m_guid(Utils::GenerateGUID())
{
    FN("VulkanIndexBuffer::VulkanIndexBuffer");
}

VulkanIndexBuffer::~VulkanIndexBuffer() { FN("VulkanIndexBuffer::~VulkanIndexBuffer"); }

bool VulkanIndexBuffer::Initialize()
{
    FN("VulkanIndexBuffer::Initialize");
    OPTICK_EVENT();

    if (m_enableShaderDeviceAdressBit)
    {
        return m_buffer.Initialize(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    }
    else
    {
        return m_buffer.Initialize(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }
}

void VulkanIndexBuffer::Bind(VkCommandBuffer commandBuffer)
{
    FN("VulkanIndexBuffer::Bind");

    LOG_TRACE("vkCmdBindIndexBuffer %s", m_guid.c_str());
    vkCmdBindIndexBuffer(commandBuffer, m_buffer.GetHandle(), 0, VulkanTypeConverter::ToVkIndexType(m_datatype));
}

void VulkanIndexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("VulkanIndexBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_buffer.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

VkDeviceAddress VulkanIndexBuffer::GetBufferDeviceAddress()
{
    FN("VulkanIndexBuffer::GetBufferDeviceAddress");

    return m_buffer.GetBufferDeviceAddress();
}

int64_t VulkanIndexBuffer::VGetSizeInBytes()
{
    FN("VulkanIndexBuffer::VGetSizeInBytes");

    return m_buffer.GetSizeInBytes();
}

BufferHint VulkanIndexBuffer::VGetUsageHint()
{
    FN("VulkanIndexBuffer::VGetUsageHint");

    return m_buffer.GetUsageHint();
}

VkBuffer VulkanIndexBuffer::GetHandle() const
{
    FN("VulkanIndexBuffer::GetHandle");

    return m_buffer.GetHandle();
}

uint32_t VulkanIndexBuffer::GetIndexCount() const
{
    FN("VulkanIndexBuffer::GetIndexCount");

    return static_cast<uint32_t>(m_buffer.GetSizeInBytes() / VertexAttributeBindingsSizes::SizeOf(m_datatype));
}

IndexBufferDatatype VulkanIndexBuffer::GetDatatype() const
{
    FN("VulkanIndexBuffer::GetDatatype");

    return m_datatype;
}

} // namespace bow