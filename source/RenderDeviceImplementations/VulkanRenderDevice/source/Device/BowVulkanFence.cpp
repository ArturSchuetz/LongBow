#include <VulkanRenderDevice/Device/BowVulkanFence.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanFence::VulkanFence() : m_logicalDevice(nullptr), m_fence(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID()) { FN("VulkanFence::VulkanFence"); }

VulkanFence::~VulkanFence()
{
    FN("VulkanFence::~VulkanFence");
    Release();
}

bool VulkanFence::Initialize(VulkanLogicalDevice *logicalDevice)
{
    FN("VulkanFence::Initialize");

    m_logicalDevice = logicalDevice;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;

    LOG_TRACE("vkCreateFence %s", m_guid.c_str());
    VkResult result = vkCreateFence(m_logicalDevice->GetHandle(), &fenceCreateInfo, nullptr, &m_fence);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanFence: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    return true;
}

void VulkanFence::Release()
{
    FN("VulkanFence::Release");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());
    if (m_fence != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyFence %s", m_guid.c_str());
        vkDestroyFence(m_logicalDevice->GetHandle(), m_fence, nullptr);
        m_fence = VK_NULL_HANDLE;
    }
}

FenceStatus VulkanFence::GetFenceStatus()
{
    FN("VulkanFence::GetFenceStatus");

    LOG_TRACE("vkGetFenceStatus %s", m_guid.c_str());
    VkResult result = vkGetFenceStatus(m_logicalDevice->GetHandle(), m_fence);
    if (result != VK_SUCCESS)
    {
        if (result == VK_NOT_READY)
        {
            return FenceStatus::NotReady;
        }
        else
        {
            LOG_ERROR("VulkanFence: %s", VulkanTypeConverter::ToString(result).c_str());
            return FenceStatus::Invalid;
        }
    }
    return FenceStatus::Ready;
}

bool VulkanFence::WaitForFence()
{
    FN("VulkanFence::WaitForFence");

    VkFence fences[] = {m_fence};

    LOG_TRACE("vkWaitForFences %s", m_guid.c_str());
    VkResult result = vkWaitForFences(m_logicalDevice->GetHandle(), 1, fences, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanFence: Failed to wait for fence: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }
    return true;
}

bool VulkanFence::ResetFence()
{
    FN("VulkanFence::ResetFence");

    VkFence fences[] = {m_fence};

    LOG_TRACE("vkResetFences %s", m_guid.c_str());
    VkResult result = vkResetFences(m_logicalDevice->GetHandle(), 1, fences);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanFence: Failed to reset fence: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }
    return true;
}

} // namespace bow