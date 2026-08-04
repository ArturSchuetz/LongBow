#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/Device/BowGraphicsWindow.h>

struct GLFWwindow;

namespace bow
{

class VulkanGraphicsWindow : public IGraphicsWindow
{
  public:
    VulkanGraphicsWindow();
    ~VulkanGraphicsWindow();

    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================
    bool Initialize(VulkanRenderDevice *device, uint32_t width, uint32_t height, const std::string &title, WindowType windowType);
    void VRelease();

    RenderContextPtr VGetContext() const;
    void VPollWindowEvents() const;

    void *VGetHandle() const;
    void VSetWindowTitle(const char *title) const;

    void VHideCursor() const;
    void VShowCursor() const;

    int VGetWidth() const;
    int VGetHeight() const;

    bool VIsVisible() const;
    bool VIsFocused() const;
    bool VShouldClose() const;

    // =========================================================================
    // Vulkan specific

    bool InitializeSurface();
    bool InitializeSwapchain();

  private:
    // you shall not copy!
    VulkanGraphicsWindow(const VulkanGraphicsWindow &) = delete;
    VulkanGraphicsWindow &operator=(const VulkanGraphicsWindow &) = delete;

    static void ResizeCallback(GLFWwindow *window, int width, int height);

    VulkanRenderDevice *m_renderDevice;
    VulkanRenderSurfacePtr m_surface;
    GLFWwindow *m_window;

    int m_Width;
    int m_Height;

    std::string m_guid;
};

} // namespace bow
