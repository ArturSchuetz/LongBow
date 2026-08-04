#include <VulkanRenderDevice/Device/Buffer/BowVulkanVertexBuffer.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBufferBase.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanVertexBuffer::VulkanVertexBuffer(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, int64_t sizeInBytes, bool enableShaderDeviceAdressBit)
    : m_logicalDevice(logicalDevice), m_enableShaderDeviceAdressBit(enableShaderDeviceAdressBit), m_buffer(m_logicalDevice, usageHint, sizeInBytes), m_guid(Utils::GenerateGUID())
{
    FN("VulkanVertexBuffer::VulkanVertexBuffer");
}

VulkanVertexBuffer::~VulkanVertexBuffer() { FN("VulkanVertexBuffer::~VulkanVertexBuffer"); }

bool VulkanVertexBuffer::Initialize()
{
    FN("VulkanVertexBuffer::Initialize");
    OPTICK_EVENT();

    if (m_enableShaderDeviceAdressBit)
    {
        return m_buffer.Initialize(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    }
    else
    {
        return m_buffer.Initialize(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }
}

void VulkanVertexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("VulkanVertexBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_buffer.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

VkDeviceAddress VulkanVertexBuffer::GetBufferDeviceAddress()
{
    FN("VulkanVertexBuffer::GetBufferDeviceAddress");

    return m_buffer.GetBufferDeviceAddress();
}

int64_t VulkanVertexBuffer::VGetSizeInBytes()
{
    FN("VulkanVertexBuffer::VGetSizeInBytes");

    return m_buffer.GetSizeInBytes();
}

BufferHint VulkanVertexBuffer::VGetUsageHint()
{
    FN("VulkanVertexBuffer::VGetUsageHint");

    return m_buffer.GetUsageHint();
}

VkBuffer VulkanVertexBuffer::GetHandle() const
{
    FN("VulkanVertexBuffer::GetHandle");

    return m_buffer.GetHandle();
}

} // namespace bow
