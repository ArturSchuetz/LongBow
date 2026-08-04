#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanBuffer::VulkanBuffer(VulkanLogicalDevice *logicalDevice) : m_logicalDevice(logicalDevice), m_useShaderDeviceAdressBit(false), m_buffer(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID()) { FN("VulkanBuffer::VulkanBuffer"); }

VulkanBuffer::~VulkanBuffer()
{
    FN("VulkanBuffer::~VulkanBuffer");

    if (m_buffer != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyBuffer %s", m_guid.c_str());
        vkDestroyBuffer(m_logicalDevice->GetHandle(), m_buffer, nullptr);
    }
}

bool VulkanBuffer::Initialize(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    FN("VulkanBuffer::Initialize");
    OPTICK_EVENT();

    m_useShaderDeviceAdressBit = (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;

    LOG_TRACE("vkCreateBuffer %s", m_guid.c_str());
    VkResult result = vkCreateBuffer(m_logicalDevice->GetHandle(), &bufferInfo, nullptr, &m_buffer);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_deviceMemory = VulkanDeviceMemoryPtr(new VulkanDeviceMemory(m_logicalDevice, this));
    if (!m_deviceMemory->AllocateMemory(properties))
    {
        LOG_ERROR("VulkanBuffer: Could not allocate memory");
        return false;
    }

    return true;
}

VulkanDeviceMemoryPtr VulkanBuffer::GetDeviceMemory()
{
    FN("VulkanBuffer::GetDeviceMemory");

    return m_deviceMemory;
}

VkDeviceAddress VulkanBuffer::GetBufferDeviceAddress()
{
    FN("VulkanBuffer::GetBufferDeviceAddress");

    if (m_useShaderDeviceAdressBit)
    {
        VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
        bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAddressInfo.pNext = nullptr;
        bufferDeviceAddressInfo.buffer = m_buffer;

        return vkGetBufferDeviceAddress(m_logicalDevice->GetHandle(), &bufferDeviceAddressInfo);
    }
    else
    {
        LOG_ERROR("VulkanBuffer: Buffer does not have VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set");
        return 0;
    }
}

bool VulkanBuffer::UseShaderDeviceAdressBit()
{
    FN("VulkanBuffer::UseShaderDeviceAdressBit");

    return m_useShaderDeviceAdressBit;
}

VkBuffer VulkanBuffer::GetHandle()
{
    FN("VulkanBuffer::GetHandle");

    return m_buffer;
}

} // namespace bow
