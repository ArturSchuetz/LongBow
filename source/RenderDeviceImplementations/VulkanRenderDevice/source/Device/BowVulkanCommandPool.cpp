#include <VulkanRenderDevice/Device/BowVulkanCommandPool.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanCommandPool::VulkanCommandPool() : m_logicalDevice(nullptr), m_commandPool(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID()) { FN("VulkanCommandPool::VulkanCommandPool"); }

VulkanCommandPool::~VulkanCommandPool()
{
    FN("VulkanCommandPool::~VulkanCommandPool");
    Release();
}

bool VulkanCommandPool::Initialize(VulkanLogicalDevice *logicalDevice, uint32_t queueFamilyIndex)
{
    FN("VulkanCommandPool::Initialize");

    m_logicalDevice = logicalDevice;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    LOG_TRACE("vkCreateCommandPool %s", m_guid.c_str());
    VkResult result = vkCreateCommandPool(m_logicalDevice->GetHandle(), &commandPoolCreateInfo, nullptr, &m_commandPool);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandPool: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    return true;
}

void VulkanCommandPool::Release()
{
    FN("VulkanCommandPool::Release");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());
    if (m_commandPool != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyCommandPool %s", m_guid.c_str());
        vkDestroyCommandPool(m_logicalDevice->GetHandle(), m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
}

VkCommandPool VulkanCommandPool::GetHandle() const
{
    FN("VulkanCommandPool::GetHandle");

    return m_commandPool;
}

VulkanCommandBufferPtr VulkanCommandPool::AllocateCommandBuffers(uint32_t count)
{
    FN("VulkanCommandPool::AllocateCommandBuffers");

    VulkanCommandBufferPtr commandBuffer = VulkanCommandBufferPtr(new VulkanCommandBuffer());
    if (!commandBuffer->Allocate(m_logicalDevice, this, count, true))
    {
        LOG_ERROR("VulkanCommandPool: Failed to allocate command buffer!");
        return VulkanCommandBufferPtr(nullptr);
    }

    return commandBuffer;
}

} // namespace bow