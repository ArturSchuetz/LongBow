#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanCommandPool.h>
#include <VulkanRenderDevice/Device/BowVulkanFence.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanCommandBuffer::VulkanCommandBuffer()
    : m_logicalDevice(nullptr), m_commandPool(nullptr), m_isPrimary(false), m_count(0), m_commandBufferStates(0, VulkanCommandBufferState::NotAllocated), m_commandBuffers(m_count), m_waitForSemaphores(0), m_guid(Utils::GenerateGUID())
{
    FN("VulkanCommandBuffer::VulkanCommandBuffer");
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    FN("VulkanCommandBuffer::~VulkanCommandBuffer");

    Release();
}

bool VulkanCommandBuffer::Allocate(VulkanLogicalDevice *logicalDevice, VulkanCommandPool *commandPool, uint32_t count, bool isPrimary)
{
    FN("VulkanCommandBuffer::Allocate");
    OPTICK_EVENT();

    m_logicalDevice = logicalDevice;
    m_commandPool = commandPool;
    m_count = count;
    m_isPrimary = isPrimary;

    Release();

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = m_commandPool->GetHandle();
    commandBufferAllocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    commandBufferAllocateInfo.commandBufferCount = m_count;

    m_commandBuffers.resize(m_count);
    LOG_TRACE("vkAllocateCommandBuffers %s", m_guid.c_str());
    VkResult result = vkAllocateCommandBuffers(m_logicalDevice->GetHandle(), &commandBufferAllocateInfo, &m_commandBuffers[0]);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_commandBufferStates.resize(m_count, VulkanCommandBufferState::Ready);
    for (auto &state : m_commandBufferStates)
    {
        state = VulkanCommandBufferState::Ready;
    }
    return true;
}

void VulkanCommandBuffer::Release()
{
    FN("VulkanCommandBuffer::Release");
    if (!m_commandBuffers.empty())
    {
        LOG_TRACE("vkFreeCommandBuffers %s", m_guid.c_str());
        vkFreeCommandBuffers(m_logicalDevice->GetHandle(), m_commandPool->GetHandle(), m_count, m_commandBuffers.data());
        m_commandBuffers.clear();
        for (auto &state : m_commandBufferStates)
        {
            state = VulkanCommandBufferState::NotAllocated;
        }
    }
}

VkCommandBuffer VulkanCommandBuffer::GetHandle(uint32_t index) const
{
    FN("VulkanCommandBuffer::GetHandle");

    if (index < m_commandBuffers.size())
    {
        return m_commandBuffers[index];
    }
    return VK_NULL_HANDLE;
}

uint32_t VulkanCommandBuffer::GetCount() const
{
    FN("VulkanCommandBuffer::GetCount");

    return m_count;
}

VulkanCommandBufferState VulkanCommandBuffer::GetState(uint32_t index) const
{
    FN("VulkanCommandBuffer::GetState");

    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    return m_commandBufferStates[index];
}

bool VulkanCommandBuffer::Begin(uint32_t index, bool isSingleUse, bool isRenderpassContinue, bool isSimultaniousUse)
{
    FN("VulkanCommandBuffer::Begin");
    OPTICK_EVENT();

    LOG_ASSERT(!m_commandBuffers.empty(), "VulkanCommandBuffer: Command buffer is not initialized!");
    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    LOG_ASSERT(m_commandBufferStates[index] == VulkanCommandBufferState::Ready, "VulkanCommandBuffer: Command buffer is not in ready state!");

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = 0;
    if (isSingleUse)
    {
        commandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    if (isRenderpassContinue)
    {
        if (m_isPrimary)
        {
            LOG_ERROR("VulkanCommandBuffer: Renderpass continue is only supported for secondary command buffers!");
            return false;
        }
        commandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }
    if (isSimultaniousUse)
    {
        commandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    LOG_TRACE("vkBeginCommandBuffer %s", m_guid.c_str());
    VkResult result = vkBeginCommandBuffer(m_commandBuffers[index], &commandBufferBeginInfo);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_commandBufferStates[index] = VulkanCommandBufferState::Recording;
    return true;
}

bool VulkanCommandBuffer::End(uint32_t index)
{
    FN("VulkanCommandBuffer::End");
    OPTICK_EVENT();

    LOG_ASSERT(!m_commandBuffers.empty(), "VulkanCommandBuffer: Command buffer is not initialized!");
    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    LOG_ASSERT(m_commandBufferStates[index] == VulkanCommandBufferState::Recording, "VulkanCommandBuffer: Command buffer is not in recording state!");

    LOG_TRACE("vkEndCommandBuffer %s", m_guid.c_str());
    VkResult result = vkEndCommandBuffer(m_commandBuffers[index]);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_commandBufferStates[index] = VulkanCommandBufferState::RecordingEnded;
    return true;
}

bool VulkanCommandBuffer::Submit(VkQueue queue, uint32_t index, VkSemaphore waitForSemaphore, VkSemaphore signalSemaphore)
{
    FN("VulkanCommandBuffer::Submit");

    LOG_ASSERT(!m_commandBuffers.empty(), "VulkanCommandBuffer: Command buffer is not initialized!");
    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    LOG_ASSERT(m_commandBufferStates[index] == VulkanCommandBufferState::RecordingEnded, "VulkanCommandBuffer: Command buffer is not in recording ended state!");

    VkSemaphore waitSemaphores[] = {waitForSemaphore};
    VkSemaphore signalSemaphores[] = {signalSemaphore};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = waitForSemaphore != nullptr ? 1 : 0;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[index];
    submitInfo.signalSemaphoreCount = signalSemaphore != nullptr ? 1 : 0;
    submitInfo.pSignalSemaphores = signalSemaphores;

    return Submit(queue, index, submitInfo);
}

bool VulkanCommandBuffer::Submit(VkQueue queue, uint32_t index, VkSubmitInfo submitInfo)
{
    FN("VulkanCommandBuffer::Submit");
    OPTICK_EVENT();
    if (GetState() == VulkanCommandBufferState::Submitted)
    {
        WaitForQueueIdle(queue);
    }

    LOG_ASSERT(!m_commandBuffers.empty(), "VulkanCommandBuffer: Command buffer is not initialized!");
    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    LOG_ASSERT(m_commandBufferStates[index] == VulkanCommandBufferState::RecordingEnded, "VulkanCommandBuffer: Command buffer is not in recording ended state!");

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[index];

    if (m_waitForSemaphores.size() > 0)
    {
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(m_waitForSemaphores.size());
        submitInfo.pWaitSemaphores = m_waitForSemaphores.data();
    }

    LOG_TRACE("vkQueueSubmit %s", m_guid.c_str());
    VkResult result = vkQueueSubmit(queue, 1, &submitInfo, nullptr);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_waitForSemaphores.clear();

    for (uint32_t i = 0; i < submitInfo.signalSemaphoreCount; i++)
    {
        m_waitForSemaphores.push_back(submitInfo.pSignalSemaphores[i]);
    }

    m_commandBufferStates[index] = VulkanCommandBufferState::Submitted;
    return true;
}

bool VulkanCommandBuffer::WaitForQueueIdle(VkQueue queue, uint32_t index)
{
    FN("VulkanCommandBuffer::WaitForQueueIdle");
    OPTICK_EVENT();

    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    LOG_ASSERT(m_commandBufferStates[index] == VulkanCommandBufferState::Submitted, "VulkanCommandBuffer: Command buffer is not in submitted state!");

    LOG_TRACE("vkQueueWaitIdle %s", m_guid.c_str());
    VkResult result = vkQueueWaitIdle(queue);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_commandBufferStates[index] = VulkanCommandBufferState::Ready;
    return true;
}

bool VulkanCommandBuffer::Reset(uint32_t index)
{
    FN("VulkanCommandBuffer::Reset");
    OPTICK_EVENT();

    LOG_ASSERT(!m_commandBuffers.empty(), "VulkanCommandBuffer: Command buffer is not initialized!");
    LOG_ASSERT(!(index >= m_commandBuffers.size()), "VulkanCommandBuffer: Index out of range!");
    LOG_ASSERT(m_commandBufferStates[index] == VulkanCommandBufferState::Submitted, "VulkanCommandBuffer: Command buffer is not in submitted state!");

    LOG_TRACE("vkResetCommandBuffer %s", m_guid.c_str());
    VkResult result = vkResetCommandBuffer(m_commandBuffers[index], 0);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanCommandBuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_commandBufferStates[index] = VulkanCommandBufferState::Ready;
    return true;
}

std::vector<VkSemaphore> VulkanCommandBuffer::GetWaitSemaphores()
{
    FN("VulkanCommandBuffer::GetWaitSemaphores");

    return m_waitForSemaphores;
}

void VulkanCommandBuffer::ClearSemaphores()
{
    FN("VulkanCommandBuffer::CleanSemaphores");

    m_waitForSemaphores.clear();
}

} // namespace bow