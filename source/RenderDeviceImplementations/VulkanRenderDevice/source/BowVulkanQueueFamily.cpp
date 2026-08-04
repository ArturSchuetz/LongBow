#include <VulkanRenderDevice/BowVulkanQueueFamily.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanQueueFamily::VulkanQueueFamily(uint32_t familyIndex, const VkQueueFamilyProperties &queue_family_properties) : m_familyIndex(), m_properties(), m_guid(Utils::GenerateGUID())
{
    FN("VulkanQueueFamily::VulkanQueueFamily");

    m_familyIndex = familyIndex;
    m_properties = queue_family_properties;
}

VulkanQueueFamily::~VulkanQueueFamily() { FN("VulkanQueueFamily::~VulkanQueueFamily"); }

uint32_t VulkanQueueFamily::GetFamilyIndex()
{
    FN("VulkanQueueFamily::GetFamilyIndex");

    return m_familyIndex;
}

uint32_t VulkanQueueFamily::GetQueueCount()
{
    FN("VulkanQueueFamily::GetQueueCount");

    return m_properties.queueCount;
}

bool VulkanQueueFamily::IsGraphicsQueue()
{
    FN("VulkanQueueFamily::IsGraphicsQueue");

    return m_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
}

bool VulkanQueueFamily::IsComputeQueue()
{
    FN("VulkanQueueFamily::IsComputeQueue");

    return m_properties.queueFlags & VK_QUEUE_COMPUTE_BIT;
}

bool VulkanQueueFamily::IsTransferQueue()
{
    FN("VulkanQueueFamily::IsTransferQueue");

    return m_properties.queueFlags & VK_QUEUE_TRANSFER_BIT;
}

bool VulkanQueueFamily::IsSparseBindingQueue()
{
    FN("VulkanQueueFamily::IsSparseBindingQueue");

    return m_properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
}

bool VulkanQueueFamily::IsProtectedQueue()
{
    FN("VulkanQueueFamily::IsProtectedQueue");

    return m_properties.queueFlags & VK_QUEUE_PROTECTED_BIT;
}

bool VulkanQueueFamily::IsPresentQueue(VkPhysicalDevice device, uint32_t familyIndex, VkSurfaceKHR surface)
{
    FN("VulkanQueueFamily::IsPresentQueue");

    VkBool32 supportsPresent = VK_FALSE;
    LOG_TRACE("vkGetPhysicalDeviceSurfaceSupportKHR %s", m_guid.c_str());
    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, familyIndex, surface, &supportsPresent);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanPhysicalDevice: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }
    return supportsPresent;
}

} // namespace bow