#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>

#include <VulkanRenderDevice/Device/BowVulkanCommandPool.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanSingleUseCommandBuffer::VulkanSingleUseCommandBuffer() : m_commandBuffer(VulkanCommandBufferPtr(new VulkanCommandBuffer())), m_queue(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID())
{
    FN("VulkanSingleUseCommandBuffer::VulkanSingleUseCommandBuffer");
}

VulkanSingleUseCommandBuffer::~VulkanSingleUseCommandBuffer()
{
    FN("VulkanSingleUseCommandBuffer::~VulkanSingleUseCommandBuffer");

    Release();
}

bool VulkanSingleUseCommandBuffer::AllocateAndBegin(VulkanLogicalDevice *logicalDevice, VulkanCommandPool *commandPool, VkQueue queue)
{
    FN("VulkanSingleUseCommandBuffer::AllocateAndBegin");
    OPTICK_EVENT();

    m_queue = queue;

    if (!m_commandBuffer->Allocate(logicalDevice, commandPool, 1, true))
    {
        return false;
    }

    if (!m_commandBuffer->Begin(0, true, false, false))
    {
        return false;
    }

    return true;
}

bool VulkanSingleUseCommandBuffer::EndAndSubmit()
{
    FN("VulkanSingleUseCommandBuffer::EndAndSubmit");
    OPTICK_EVENT();

    if (!m_commandBuffer->End())
    {
        return false;
    }

    if (!m_commandBuffer->Submit(m_queue))
    {
        return false;
    }

    if (!m_commandBuffer->WaitForQueueIdle(m_queue))
    {
        return false;
    }

    return true;
}

void VulkanSingleUseCommandBuffer::Release()
{
    FN("VulkanSingleUseCommandBuffer::Release");

    m_commandBuffer->Release();
}

VkCommandBuffer VulkanSingleUseCommandBuffer::GetHandle() const
{
    FN("VulkanSingleUseCommandBuffer::GetHandle");

    return m_commandBuffer->GetHandle();
}

VulkanCommandBufferState VulkanSingleUseCommandBuffer::GetState() const
{
    FN("VulkanSingleUseCommandBuffer::GetState");

    return m_commandBuffer->GetState();
}

} // namespace bow