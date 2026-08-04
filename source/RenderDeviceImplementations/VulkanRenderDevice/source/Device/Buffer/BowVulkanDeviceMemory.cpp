#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanDeviceMemory::VulkanDeviceMemory(VulkanLogicalDevice *logicalDevice, VulkanBuffer *buffer) : m_logicalDevice(logicalDevice), m_buffer(buffer), m_deviceMemory(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID())
{
    FN("VulkanDeviceMemory::VulkanDeviceMemory");
}

VulkanDeviceMemory::~VulkanDeviceMemory()
{
    FN("VulkanDeviceMemory::~VulkanDeviceMemory");

    if (m_deviceMemory != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkFreeMemory %s", m_guid.c_str());
        vkFreeMemory(m_logicalDevice->GetHandle(), m_deviceMemory, nullptr);
    }
}

bool VulkanDeviceMemory::AllocateMemory(VkMemoryPropertyFlags properties)
{
    FN("VulkanDeviceMemory::AllocateMemory");
    OPTICK_EVENT();

    LOG_ASSERT(m_deviceMemory == VK_NULL_HANDLE, "VulkanDeviceMemory: Memory already allocated");

    VkMemoryRequirements memRequirements;
    LOG_TRACE("vkGetBufferMemoryRequirements %s", m_guid.c_str());
    vkGetBufferMemoryRequirements(m_logicalDevice->GetHandle(), m_buffer->GetHandle(), &memRequirements);

    const void *pMemoryAllocateInfoNext = nullptr;

    VkMemoryAllocateFlagsInfo allocateFlagsInfo = {};
    allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
    if (m_buffer->UseShaderDeviceAdressBit())
    {
        pMemoryAllocateInfoNext = &allocateFlagsInfo;
    }

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = pMemoryAllocateInfoNext;
    memoryAllocateInfo.allocationSize = memRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = m_logicalDevice->GetParentPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, properties);

    LOG_TRACE("vkAllocateMemory %s", m_guid.c_str());
    VkResult result = vkAllocateMemory(m_logicalDevice->GetHandle(), &memoryAllocateInfo, nullptr, &m_deviceMemory);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    LOG_TRACE("vkBindBufferMemory %s", m_guid.c_str());
    result = vkBindBufferMemory(m_logicalDevice->GetHandle(), m_buffer->GetHandle(), m_deviceMemory, 0);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    return true;
}

VkDeviceMemory VulkanDeviceMemory::GetHandle()
{
    FN("VulkanDeviceMemory::GetHandle");

    return m_deviceMemory;
}

} // namespace bow
