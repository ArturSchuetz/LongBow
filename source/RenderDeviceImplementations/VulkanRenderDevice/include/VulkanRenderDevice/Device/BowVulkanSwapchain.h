#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanSwapchain
{
  public:
    VulkanSwapchain(VulkanLogicalDevice *logicalDevice, VulkanRenderSurface *renderSurface);
    ~VulkanSwapchain();

    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================
    bool Initialize(int width, int height, bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true);
    void VRelease();

    VkSwapchainKHR GetHandle() const;

    uint32_t GetCurrentImageIndex();

    VulkanFramebufferPtr GetCurrentFramebuffer();
    VulkanSemaphorePtr GetCurrentImageAvailableSemaphore();

    VulkanTexture2DPtr GetCurrentRenderTarget();

    VkResult SwapBuffers(bool vsync);

  private:
    // you shall not copy!
    VulkanSwapchain(const VulkanSwapchain &) = delete;
    VulkanSwapchain &operator=(const VulkanSwapchain &) = delete;

    VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> &modes);
    VkExtent2D ChooseSurfaceExtent(const VkSurfaceCapabilitiesKHR &caps, int width, int height);

    VulkanLogicalDevice *m_logicalDevice;
    VulkanRenderSurface *m_renderSurface;

    VkRect2D m_renderArea;
    VkSwapchainKHR m_swapChain;
    std::vector<VulkanFramebufferPtr> m_framebuffers;

    std::vector<VulkanTexture2DPtr> m_swapChainRenderTargets;
    std::vector<VulkanTexture2DPtr> m_swapChainDepthTargets;

    VulkanSemaphorePtr m_imageAvailableSemaphore;
    std::vector<VulkanSemaphorePtr> m_acquireSemaphores;

    uint32_t m_currentImageIndex;

    std::string m_guid;
};

} // namespace bow
