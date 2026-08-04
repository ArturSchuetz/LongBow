#include <VulkanRenderDevice/Device/Buffer/BowVulkanUniformBuffer.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanUniformBuffer::VulkanUniformBuffer(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, int64_t sizeInBytes)
    : m_logicalDevice(logicalDevice), m_sizeInBytes(sizeInBytes), m_usageHint(usageHint), m_buffer(nullptr), m_deviceMemory(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID())
{
    FN("VulkanUniformBuffer::VulkanUniformBuffer");
}

VulkanUniformBuffer::~VulkanUniformBuffer() { FN("VulkanUniformBuffer::~VulkanUniformBuffer"); }

bool VulkanUniformBuffer::Initialize()
{
    FN("VulkanUniformBuffer::Initialize");
    OPTICK_EVENT();

    LOG_ASSERT(m_buffer == nullptr, "VulkanUniformBuffer: Buffer already initialized");

    m_buffer = VulkanBufferPtr(new VulkanBuffer(m_logicalDevice));
    bool success = m_buffer->Initialize(m_sizeInBytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VulkanTypeConverter::ToVkBufferUsageFlags(m_usageHint), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (!success)
    {
        LOG_FATAL("VulkanUniformBuffer: Failed to initialize buffer");
        return false;
    }

    m_deviceMemory = m_buffer->GetDeviceMemory();
    LOG_ASSERT(m_deviceMemory != nullptr, "VulkanUniformBuffer: Failed to allocate memory");

    return true;
}

void VulkanUniformBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("VulkanUniformBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    void *bufferInDeviceMemory;
    LOG_TRACE("vkMapMemory %s", m_guid.c_str());
    VkResult result = vkMapMemory(m_logicalDevice->GetHandle(), m_deviceMemory->GetHandle(), destinationOffsetInBytes, static_cast<VkDeviceSize>(lengthInBytes), 0, &bufferInDeviceMemory);
    if (result != VK_SUCCESS)
    {
        LOG_FATAL("VulkanUniformBuffer: %s", VulkanTypeConverter::ToString(resukt));
        return;
    }
    memcpy(bufferInDeviceMemory, bufferInSystemMemory, (size_t)lengthInBytes);
    LOG_TRACE("vkUnmapMemory %s", m_guid.c_str());
    vkUnmapMemory(m_logicalDevice->GetHandle(), m_deviceMemory->GetHandle());
}

std::shared_ptr<void> VulkanUniformBuffer::VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes)
{
    FN("VulkanUniformBuffer::VCopyToSystemMemory");
    OPTICK_EVENT();

    VulkanDeviceMemoryPtr deviceMemory = m_buffer->GetDeviceMemory();
    LOG_ASSERT(deviceMemory != nullptr, "VulkanUniformBuffer: Device memory not allocated");

    std::shared_ptr<void> bufferInSystemMemory = std::shared_ptr<void>(new int8_t[sizeInBytes]);

    void *bufferInDeviceMemory;
    LOG_TRACE("vkMapMemory %s", m_guid.c_str());
    VkResult result = vkMapMemory(m_logicalDevice->GetHandle(), m_deviceMemory->GetHandle(), offsetInBytes, sizeInBytes, 0, &bufferInDeviceMemory);
    if (result != VK_SUCCESS)
    {
        LOG_FATAL("VulkanUniformBuffer: %s", VulkanTypeConverter::ToString(resukt));
        return nullptr;
    }

    memcpy(bufferInSystemMemory.get(), bufferInDeviceMemory, (size_t)sizeInBytes);
    LOG_TRACE("vkUnmapMemory %s", m_guid.c_str());
    vkUnmapMemory(m_logicalDevice->GetHandle(), m_deviceMemory->GetHandle());

    return bufferInSystemMemory;
}

int64_t VulkanUniformBuffer::VGetSizeInBytes()
{
    FN("VulkanUniformBuffer::VGetSizeInBytes");

    return m_sizeInBytes;
}

BufferHint VulkanUniformBuffer::VGetUsageHint()
{
    FN("VulkanUniformBuffer::VGetUsageHint");

    return m_usageHint;
}

VkBuffer VulkanUniformBuffer::GetHandle() const
{
    FN("VulkanUniformBuffer::GetHandle");

    return m_buffer->GetHandle();
}

VulkanDeviceMemoryPtr VulkanUniformBuffer::GetDeviceMemory() const
{
    FN("VulkanUniformBuffer::GetDeviceMemory");

    return m_deviceMemory;
}

uint32_t VulkanUniformBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    FN("VulkanUniformBuffer::FindMemoryType");

    VkPhysicalDeviceMemoryProperties memProperties;
    LOG_TRACE("vkGetPhysicalDeviceMemoryProperties %s", m_guid.c_str());
    vkGetPhysicalDeviceMemoryProperties(m_logicalDevice->GetParentPhysicalDevice()->GetHandle(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    LOG_FATAL("VulkanUniformBuffer: Failed to find suitable memory type");
}

} // namespace bow
