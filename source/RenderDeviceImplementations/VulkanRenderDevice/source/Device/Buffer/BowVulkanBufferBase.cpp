#include <VulkanRenderDevice/Device/Buffer/BowVulkanBufferBase.h>

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

VulkanBufferBase::VulkanBufferBase(VulkanLogicalDevice *logicalDevice, BufferHint usageHint, int64_t sizeInBytes) : m_logicalDevice(logicalDevice), m_sizeInBytes(sizeInBytes), m_usageHint(usageHint), m_buffer(nullptr), m_guid(Utils::GenerateGUID())
{
    FN("VulkanBufferBase::VulkanBufferBase");
}

VulkanBufferBase::~VulkanBufferBase() { FN("VulkanBufferBase::~VulkanBufferBase"); }

bool VulkanBufferBase::Initialize(VkBufferUsageFlags usage)
{
    FN("VulkanBufferBase::Initialize");
    OPTICK_EVENT();

    LOG_ASSERT(m_buffer == nullptr, "VulkanVertexBuffer: Buffer already initialized");

    m_useShaderDeviceAdressBit = (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0;

    m_buffer = VulkanBufferPtr(new VulkanBuffer(m_logicalDevice));
    bool success = m_buffer->Initialize(m_sizeInBytes, usage | VulkanTypeConverter::ToVkBufferUsageFlags(m_usageHint), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!success)
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to initialize buffer");
        return false;
    }

    return true;
}

void VulkanBufferBase::CopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("VulkanBufferBase::CopyFromSystemMemory");
    OPTICK_EVENT();

    LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");
    LOG_ASSERT(!(lengthInBytes < 0), "lengthInBytes must be greater than or equal to zero.");

    VkDeviceSize bufferSize = static_cast<VkDeviceSize>(lengthInBytes);

    VulkanBufferPtr stagingBuffer = VulkanBufferPtr(new VulkanBuffer(m_logicalDevice));
    bool success = stagingBuffer->Initialize(m_sizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (!success)
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to initialize staging buffer");
        return;
    }

    VulkanDeviceMemoryPtr stagingBufferMemory = stagingBuffer->GetDeviceMemory();
    LOG_ASSERT(stagingBufferMemory != nullptr, "VulkanIndexBuffer: Failed to allocate memory");

    void *data;
    LOG_TRACE("vkMapMemory %s", m_guid.c_str());
    VkResult result = vkMapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle(), 0, bufferSize, 0, &data);
    if (result != VK_SUCCESS)
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to map staging buffer memory");
        return;
    }

    memcpy(data, bufferInSystemMemory, (size_t)lengthInBytes);
    LOG_TRACE("vkUnmapMemory %s", m_guid.c_str());
    vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle());

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseTransferCommandBuffer();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = destinationOffsetInBytes;
    copyRegion.size = bufferSize;
    LOG_TRACE("vkCmdCopyBuffer %s", m_guid.c_str());
    vkCmdCopyBuffer(singleTimeCommandBuffer->GetHandle(), stagingBuffer->GetHandle(), m_buffer->GetHandle(), 1, &copyRegion);

    if (!singleTimeCommandBuffer->EndAndSubmit())
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to end and submit single time command buffer");
        return;
    }
}

std::shared_ptr<void> VulkanBufferBase::CopyToSystemMemory(int64_t offsetInBytes, int64_t lengthInBytes)
{
    FN("VulkanBufferBase::CopyToSystemMemory");
    OPTICK_EVENT();

    LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(lengthInBytes <= 0), "lengthInBytes must be greater than zero.");
    LOG_ASSERT(!(offsetInBytes + lengthInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");

    VkDeviceSize bufferSize = static_cast<VkDeviceSize>(lengthInBytes);

    VulkanBufferPtr stagingBuffer = VulkanBufferPtr(new VulkanBuffer(m_logicalDevice));
    bool success = stagingBuffer->Initialize(m_sizeInBytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (!success)
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to initialize staging buffer");
        return nullptr;
    }

    VulkanDeviceMemoryPtr stagingBufferMemory = stagingBuffer->GetDeviceMemory();
    LOG_ASSERT(stagingBufferMemory != nullptr, "VulkanIndexBuffer: Failed to allocate memory");

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseTransferCommandBuffer();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = offsetInBytes;
    copyRegion.dstOffset = 0;
    copyRegion.size = bufferSize;
    LOG_TRACE("vkCmdCopyBuffer %s", m_guid.c_str());
    vkCmdCopyBuffer(singleTimeCommandBuffer->GetHandle(), m_buffer->GetHandle(), stagingBuffer->GetHandle(), 1, &copyRegion);

    if (!singleTimeCommandBuffer->EndAndSubmit())
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to end and submit single time command buffer");
        return nullptr;
    }

    void *data;
    LOG_TRACE("vkMapMemory %s", m_guid.c_str());
    VkResult result = vkMapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle(), 0, bufferSize, 0, &data);
    if (result != VK_SUCCESS)
    {
        LOG_FATAL("VulkanVertexBuffer: Failed to map staging buffer memory");
        return nullptr;
    }

    void *bufferInSystemMemory = malloc(lengthInBytes);
    memcpy(bufferInSystemMemory, data, (size_t)lengthInBytes);

    LOG_TRACE("vkUnmapMemory %s", m_guid.c_str());
    vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle());

    return std::shared_ptr<void>(bufferInSystemMemory, [](void *ptr) { free(ptr); });
}

VkDeviceAddress VulkanBufferBase::GetBufferDeviceAddress()
{
    FN("VulkanBufferBase::GetBufferDeviceAddress");

    if (m_useShaderDeviceAdressBit)
    {
        VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
        bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAddressInfo.pNext = nullptr;
        bufferDeviceAddressInfo.buffer = m_buffer->GetHandle();

        return vkGetBufferDeviceAddress(m_logicalDevice->GetHandle(), &bufferDeviceAddressInfo);
    }
    else
    {
        LOG_ERROR("VulkanBufferBase: Buffer does not have VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set");
        return 0;
    }
}

int64_t VulkanBufferBase::GetSizeInBytes() const
{
    FN("VulkanBufferBase::GetSizeInBytes");

    return m_sizeInBytes;
}

BufferHint VulkanBufferBase::GetUsageHint() const
{
    FN("VulkanBufferBase::GetUsageHint");

    return m_usageHint;
}

VkBuffer VulkanBufferBase::GetHandle() const
{
    FN("VulkanBufferBase::GetHandle");

    return m_buffer->GetHandle();
}

} // namespace bow
