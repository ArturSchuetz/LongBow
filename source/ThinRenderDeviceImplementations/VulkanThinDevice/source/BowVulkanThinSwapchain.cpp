#include <VulkanThinDevice/BowVulkanThinObjects.h>

#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

VulkanThinSwapchain::VulkanThinSwapchain(VulkanThinDevice *device, void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount)
    : m_device(device), m_surface(VK_NULL_HANDLE), m_swapchain(VK_NULL_HANDLE), m_format(VK_FORMAT_UNDEFINED), m_width(width), m_height(height), m_currentImage(0), m_frame(0)
{
    FN("VulkanThinSwapchain::VulkanThinSwapchain");

    if (!CreateSurface(nativeWindowHandle))
    {
        return;
    }

    CreateSwapchain(imageCount);
}

VulkanThinSwapchain::~VulkanThinSwapchain()
{
    FN("VulkanThinSwapchain::~VulkanThinSwapchain");

    DestroySwapchain();

    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_device->GetInstance(), m_surface, nullptr);
    }
}

bool VulkanThinSwapchain::CreateSurface(void *nativeWindowHandle)
{
    FN("VulkanThinSwapchain::CreateSurface");

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    createInfo.hinstance = GetModuleHandle(nullptr);
    createInfo.hwnd = (HWND)nativeWindowHandle;

    return VulkanCheck(vkCreateWin32SurfaceKHR(m_device->GetInstance(), &createInfo, nullptr, &m_surface), "vkCreateWin32SurfaceKHR");
#else
    LOG_ERROR("Only Win32 surfaces are implemented so far.");
    return false;
#endif
}

bool VulkanThinSwapchain::CreateSwapchain(uint32_t imageCount)
{
    FN("VulkanThinSwapchain::CreateSwapchain");

    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->GetPhysicalDevice(), m_surface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->GetPhysicalDevice(), m_surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->GetPhysicalDevice(), m_surface, &formatCount, formats.data());

    if (formats.empty())
    {
        LOG_ERROR("The surface reports no supported formats.");
        return false;
    }

    // Prefer a plain BGRA8 surface: it is what desktop compositors want, and
    // an sRGB one would double-apply the conversion the examples already do.
    VkSurfaceFormatKHR chosen = formats[0];
    for (const VkSurfaceFormatKHR &format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            chosen = format;
            break;
        }
    }
    m_format = chosen.format;

    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        m_width = capabilities.currentExtent.width;
        m_height = capabilities.currentExtent.height;
    }

    uint32_t count = imageCount;
    if (count < capabilities.minImageCount)
    {
        count = capabilities.minImageCount;
    }
    if (capabilities.maxImageCount > 0 && count > capabilities.maxImageCount)
    {
        count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    createInfo.surface = m_surface;
    createInfo.minImageCount = count;
    createInfo.imageFormat = chosen.format;
    createInfo.imageColorSpace = chosen.colorSpace;
    createInfo.imageExtent.width = m_width;
    createInfo.imageExtent.height = m_height;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // FIFO is the only mode guaranteed to be present, and it is what vsync
    // means here. VPresent chooses between waiting and not by the value it
    // passes, not by recreating the swapchain.
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;

    if (!VulkanCheck(vkCreateSwapchainKHR(m_device->GetHandle(), &createInfo, nullptr, &m_swapchain), "vkCreateSwapchainKHR"))
    {
        return false;
    }

    uint32_t actualCount = 0;
    vkGetSwapchainImagesKHR(m_device->GetHandle(), m_swapchain, &actualCount, nullptr);
    std::vector<VkImage> images(actualCount);
    vkGetSwapchainImagesKHR(m_device->GetHandle(), m_swapchain, &actualCount, images.data());

    ThinTextureDescription description;
    description.width = m_width;
    description.height = m_height;
    description.format = VulkanThinTypes::FromVkFormat(m_format);
    description.renderTarget = true;
    description.copyDestination = true;

    m_images.clear();
    for (VkImage image : images)
    {
        m_images.push_back(std::make_shared<VulkanThinTexture>(m_device, image, description));
    }

    // Acquire and present still need binary semaphores: this is the one place
    // Vulkan does not accept a timeline one, which is why IThinSwapchain owns
    // them rather than exposing them.
    m_acquireSemaphores.resize(m_images.size());
    m_presentSemaphores.resize(m_images.size());

    VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (size_t i = 0; i < m_images.size(); ++i)
    {
        vkCreateSemaphore(m_device->GetHandle(), &semaphoreInfo, nullptr, &m_acquireSemaphores[i]);
        vkCreateSemaphore(m_device->GetHandle(), &semaphoreInfo, nullptr, &m_presentSemaphores[i]);
    }

    return true;
}

void VulkanThinSwapchain::DestroySwapchain()
{
    for (VkSemaphore semaphore : m_acquireSemaphores)
    {
        vkDestroySemaphore(m_device->GetHandle(), semaphore, nullptr);
    }
    for (VkSemaphore semaphore : m_presentSemaphores)
    {
        vkDestroySemaphore(m_device->GetHandle(), semaphore, nullptr);
    }
    m_acquireSemaphores.clear();
    m_presentSemaphores.clear();

    m_images.clear();

    if (m_swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device->GetHandle(), m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

uint32_t VulkanThinSwapchain::VAcquireNextImage()
{
    FN("VulkanThinSwapchain::VAcquireNextImage");

    if (m_swapchain == VK_NULL_HANDLE)
    {
        return 0;
    }

    const VkSemaphore semaphore = m_acquireSemaphores[m_frame % m_acquireSemaphores.size()];
    const VkResult result = vkAcquireNextImageKHR(m_device->GetHandle(), m_swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &m_currentImage);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // The window changed size behind our back; rebuilding is the caller's
        // job through VResize, so report the stale index rather than crash.
        LOG_WARNING("The swapchain is out of date and needs resizing.");
    }

    return m_currentImage;
}

ThinTexturePtr VulkanThinSwapchain::VGetImage(uint32_t index) const { return (index < m_images.size()) ? m_images[index] : nullptr; }

ThinFormat VulkanThinSwapchain::VGetFormat() const { return VulkanThinTypes::FromVkFormat(m_format); }

void VulkanThinSwapchain::VPresent(bool /*vsync*/)
{
    FN("VulkanThinSwapchain::VPresent");

    if (m_swapchain == VK_NULL_HANDLE)
    {
        return;
    }

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &m_currentImage;

    ThinQueuePtr queue = m_device->VGetQueue(ThinQueueType::Graphics);
    VkQueue vkQueue = static_cast<VulkanThinQueue *>(queue.get())->GetHandle();

    const VkResult result = vkQueuePresentKHR(vkQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        LOG_WARNING("Presentation reports the swapchain is out of date.");
    }

    ++m_frame;
}

void VulkanThinSwapchain::VResize(uint32_t width, uint32_t height)
{
    FN("VulkanThinSwapchain::VResize");

    if (width == 0 || height == 0)
    {
        return;
    }

    vkDeviceWaitIdle(m_device->GetHandle());

    const uint32_t imageCount = (uint32_t)m_images.size();
    DestroySwapchain();

    m_width = width;
    m_height = height;
    CreateSwapchain(imageCount > 0 ? imageCount : 2);
}

} // namespace bow
