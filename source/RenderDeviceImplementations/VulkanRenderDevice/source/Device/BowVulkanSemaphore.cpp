#include <VulkanRenderDevice/Device/BowVulkanSemaphore.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanSemaphore::VulkanSemaphore() : m_logicalDevice(nullptr), m_semaphore(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID()) { FN("VulkanSemaphore::VulkanSemaphore"); }

VulkanSemaphore::~VulkanSemaphore()
{
    FN("VulkanSemaphore::~VulkanSemaphore");

    Release();
}

bool VulkanSemaphore::Initialize(VulkanLogicalDevice *logicalDevice)
{
    FN("VulkanSemaphore::Initialize");

    m_logicalDevice = logicalDevice;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    LOG_TRACE("vkCreateSemaphore %s", m_guid.c_str());
    VkResult result = vkCreateSemaphore(m_logicalDevice->GetHandle(), &semaphoreInfo, nullptr, &m_semaphore);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanSemaphore: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    return true;
}

void VulkanSemaphore::Release()
{
    FN("VulkanSemaphore::Release");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());
    if (m_semaphore != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroySemaphore %s", m_guid.c_str());
        vkDestroySemaphore(m_logicalDevice->GetHandle(), m_semaphore, nullptr);
        m_semaphore = VK_NULL_HANDLE;
    }
}

} // namespace bow