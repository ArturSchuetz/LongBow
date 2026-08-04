#include <VulkanRenderDevice/Device/BowVulkanSwapchain.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanRenderDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanCommandPool.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/BowVulkanRenderSurface.h>
#include <VulkanRenderDevice/Device/BowVulkanSemaphore.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanColorAttachments.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanFramebuffer.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanRenderPass.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>

#include <RenderDevice/Device/BowGraphicsWindow.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanSwapchain::VulkanSwapchain(VulkanLogicalDevice *logicalDevice, VulkanRenderSurface *renderSurface)
    : m_logicalDevice(logicalDevice), m_renderSurface(renderSurface), m_renderArea(), m_swapChain(VK_NULL_HANDLE), m_framebuffers(0, nullptr), m_swapChainRenderTargets(0, nullptr), m_acquireSemaphores(0, nullptr), m_currentImageIndex(-1),
      m_guid(Utils::GenerateGUID())
{
    FN("VulkanSwapchain::VulkanSwapchain");
}

VulkanSwapchain::~VulkanSwapchain()
{
    FN("VulkanSwapchain::~VulkanSwapchain");
    VRelease();
}

bool VulkanSwapchain::Initialize(int width, int height, bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer)
{
    FN("VulkanSwapchain::Initialize");

    // Take our selected gpu and pick three things.
    // 1.) Surface format as described earlier.
    // 2.) Present mode. Again refer to documentation I shared.
    // 3.) Surface extent is basically just the size ( width, height ) of the
    // image.
    m_logicalDevice->GetParentPhysicalDevice()->CheckSurfaceCapabilities(m_renderSurface->GetHandle());

    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(m_logicalDevice->GetParentPhysicalDevice()->GetSurfaceFormats());
    VkPresentModeKHR presentMode = ChoosePresentMode(m_logicalDevice->GetParentPhysicalDevice()->GetPresentModes());
    VkSurfaceCapabilitiesKHR capabilities = m_logicalDevice->GetParentPhysicalDevice()->GetSurfaceCapabilities();
    VkExtent2D extent = ChooseSurfaceExtent(capabilities, width, height);

    m_renderArea.offset = {0, 0};
    m_renderArea.extent = extent;

    uint32_t desiredImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && desiredImageCount > capabilities.maxImageCount)
    {
        desiredImageCount = capabilities.maxImageCount;
    }

    // Details for creating the swapchain
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.pNext = nullptr;
    swapChainCreateInfo.flags = 0;
    swapChainCreateInfo.surface = m_renderSurface->GetHandle();
    swapChainCreateInfo.minImageCount = desiredImageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Handle the case where the graphics and present queues are different
    if (m_logicalDevice->GetGraphicsQueueFamilyIndex() != m_logicalDevice->GetPresentQueueFamilyIndex())
    {
        uint32_t queueFamilyIndices[] = {m_logicalDevice->GetGraphicsQueueFamilyIndex(), m_logicalDevice->GetPresentQueueFamilyIndex()};

        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    // Handle present mode and other swapchain configurations
    swapChainCreateInfo.preTransform = capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create the swapchain
    VkBool32 m_presentQueueSupportsPresent = VK_FALSE;
    LOG_TRACE("vkGetPhysicalDeviceSurfaceSupportKHR %s", m_guid.c_str());
    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(m_logicalDevice->GetParentPhysicalDevice()->GetHandle(), m_logicalDevice->GetPresentQueueFamilyIndex(), m_renderSurface->GetHandle(), &m_presentQueueSupportsPresent);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanSwapchain: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    if (m_presentQueueSupportsPresent == VK_FALSE)
    {
        LOG_ERROR("VulkanSwapchain: Present queue does not support present.");
        return false;
    }

    LOG_TRACE("vkCreateSwapchainKHR %s", m_guid.c_str());
    result = vkCreateSwapchainKHR(m_logicalDevice->GetHandle(), &swapChainCreateInfo, nullptr, &m_swapChain);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanSwapchain: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    // Get swapchain images
    uint32_t imageCount;
    LOG_TRACE("vkGetSwapchainImagesKHR %s", m_guid.c_str());
    vkGetSwapchainImagesKHR(m_logicalDevice->GetHandle(), m_swapChain, &imageCount, nullptr);
    std::vector<VkImage> swapChainImages(imageCount);
    LOG_TRACE("vkGetSwapchainImagesKHR %s", m_guid.c_str());
    vkGetSwapchainImagesKHR(m_logicalDevice->GetHandle(), m_swapChain, &imageCount, swapChainImages.data());

    m_imageAvailableSemaphore = m_logicalDevice->CreateSemaphoreA();
    m_acquireSemaphores.resize(imageCount);

    // Create image views
    m_swapChainRenderTargets.resize(imageCount);
    m_swapChainDepthTargets.resize(imageCount);
    m_framebuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        m_acquireSemaphores[i] = m_logicalDevice->CreateSemaphoreA();

        Texture2DDescription colorDescription(std::max((int)extent.width, 1), std::max((int)extent.height, 1), VulkanTypeConverter::ToTextureFormat(surfaceFormat.format));
        m_swapChainRenderTargets[i] = VulkanTexture2DPtr(new VulkanTexture2D(m_logicalDevice, colorDescription, swapChainImages[i]));
        bool success = m_swapChainRenderTargets[i]->Initialize();
        LOG_ASSERT(success, "VulkanSwapchain: Swapchain render target could not be initialized!");
        if (!success)
            return false;

        Texture2DDescription depthDescription = Texture2DDescription(std::max((int)extent.width, 1), std::max((int)extent.height, 1), VulkanTypeConverter::ToTextureFormat(m_logicalDevice->GetParentPhysicalDevice()->FindSupportedDepthFormat()));
        m_swapChainDepthTargets[i] = VulkanTexture2DPtr(new VulkanTexture2D(m_logicalDevice, depthDescription));
        success = m_swapChainDepthTargets[i]->Initialize();
        LOG_ASSERT(success, "VulkanSwapchain: Swapchain depth target could not be initialized!");
        if (!success)
            return false;

        m_framebuffers[i] = VulkanFramebufferPtr(new VulkanFramebuffer(m_logicalDevice));
        m_framebuffers[i]->VSetColorAttachment(0, m_swapChainRenderTargets[i]);
        if (m_swapChainDepthTargets[i]->IsStencilFormat())
        {
            m_framebuffers[i]->VSetDepthStencilAttachment(m_swapChainDepthTargets[i]);
        }
        else if (m_swapChainDepthTargets[i]->IsDepthFormat())
        {
            m_framebuffers[i]->VSetDepthAttachment(m_swapChainDepthTargets[i]);
        }
        else
        {
            LOG_FATAL("VulkanSwapchain: Depth stencil format not supported!");
        }

        LOG_ASSERT(success, "VulkanSwapchain: Framebuffer could not be initialized!");
        if (!success)
            return false;
    }

    return true;
}

void VulkanSwapchain::VRelease()
{
    FN("VulkanSwapchain::VRelease");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());

    if (m_imageAvailableSemaphore != nullptr)
    {
        m_imageAvailableSemaphore.reset();
        m_imageAvailableSemaphore = nullptr;
    }

    for (auto framebuffer : m_framebuffers)
    {
        framebuffer->VRelease();
    }
    m_framebuffers.clear();

    for (auto swapChainRenderTargets : m_swapChainRenderTargets)
    {
        swapChainRenderTargets->VRelease();
    }
    m_swapChainRenderTargets.clear();

    if (m_swapChain != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroySwapchainKHR %s", m_guid.c_str());
        vkDestroySwapchainKHR(m_logicalDevice->GetHandle(), m_swapChain, nullptr);
        m_swapChain = VK_NULL_HANDLE;
    }
}

VkSwapchainKHR VulkanSwapchain::GetHandle() const
{
    FN("VulkanSwapchain::GetHandle");

    return m_swapChain;
}

uint32_t VulkanSwapchain::GetCurrentImageIndex()
{
    FN("VulkanSwapchain::GetCurrentImageIndex");

    if (m_currentImageIndex == std::numeric_limits<uint32_t>::max())
    {
        uint32_t imageIndex;
        LOG_TRACE("vkAcquireNextImageKHR %s", m_guid.c_str());
        VkResult result = vkAcquireNextImageKHR(m_logicalDevice->GetHandle(), m_swapChain, UINT64_MAX, m_imageAvailableSemaphore->GetHandle(), VK_NULL_HANDLE, &imageIndex);
        if (result != VK_SUCCESS)
        {
            LOG_FATAL("VulkanSwapchain: %s", VulkanTypeConverter::ToString(result).c_str());
            return std::numeric_limits<uint32_t>::max();
        }

        m_currentImageIndex = imageIndex;
    }

    return m_currentImageIndex;
}

VulkanFramebufferPtr VulkanSwapchain::GetCurrentFramebuffer()
{
    FN("VulkanSwapchain::GetCurrentFramebuffer");

    uint32_t imageIndex = GetCurrentImageIndex();
    return m_framebuffers[imageIndex];
}

VulkanSemaphorePtr VulkanSwapchain::GetCurrentImageAvailableSemaphore()
{
    FN("VulkanSwapchain::GetCurrentImageAvailableSemaphore");

    return m_imageAvailableSemaphore;
}

VulkanTexture2DPtr VulkanSwapchain::GetCurrentRenderTarget()
{
    FN("VulkanSwapchain::GetCurrentRenderTarget");

    uint32_t imageIndex = GetCurrentImageIndex();
    return m_swapChainRenderTargets[imageIndex];
}

VkResult VulkanSwapchain::SwapBuffers(bool vsync)
{
    FN("VulkanSwapchain::SwapBuffers");

    uint32_t imageIndex = GetCurrentImageIndex();

    VulkanCommandBufferPtr currentComandBuffer = m_framebuffers[imageIndex]->GetGraphicsCommandBuffer();

    std::vector<VkSemaphore> waitForSemaphores = currentComandBuffer->GetWaitSemaphores();

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = waitForSemaphores.size();
    presentInfo.pWaitSemaphores = waitForSemaphores.data();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapChain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    LOG_TRACE("vkQueuePresentKHR %s", m_guid.c_str());
    VkResult result = vkQueuePresentKHR(m_logicalDevice->GetPresentQueue(), &presentInfo);
    if (result != VK_SUCCESS)
    {
        if (result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR)
            LOG_ERROR("VulkanSwapchain: %s", VulkanTypeConverter::ToString(result).c_str());
        return result;
    }

    currentComandBuffer->ClearSemaphores();

    LOG_TRACE("vkQueueWaitIdle %s", m_guid.c_str());
    vkQueueWaitIdle(m_logicalDevice->GetPresentQueue());

    m_currentImageIndex = std::numeric_limits<uint32_t>::max();
    return result;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
{
    FN("VulkanSwapchain::ChooseSurfaceFormat");

    if (formats.empty())
    {
        LOG_ERROR("No available surface formats.");
        return {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    // If Vulkan returned an unknown format, then just force what we want.
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    // Favor 32 bit rgba and srgb nonlinear colorspace
    for (int i = 0; i < formats.size(); ++i)
    {
        VkSurfaceFormatKHR format = formats[i];
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    // If all else fails, just return what's available
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR> &modes)
{
    FN("VulkanSwapchain::ChoosePresentMode");

    const VkPresentModeKHR desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;

    // Favor looking for mailbox mode.
    for (int i = 0; i < modes.size(); ++i)
    {
        if (modes[i] == desiredMode)
        {
            return desiredMode;
        }
    }

    // If we couldn't find mailbox, then default to FIFO which is always
    // available.
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseSurfaceExtent(const VkSurfaceCapabilitiesKHR &capabilities, int screenWidth, int screenHeight)
{
    FN("VulkanSwapchain::ChooseSurfaceExtent");

    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {static_cast<uint32_t>(screenWidth), static_cast<uint32_t>(screenHeight)};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

} // namespace bow
