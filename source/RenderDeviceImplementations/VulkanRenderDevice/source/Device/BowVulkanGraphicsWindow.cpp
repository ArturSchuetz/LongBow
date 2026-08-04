#include <VulkanRenderDevice/Device/BowVulkanGraphicsWindow.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanQueueFamily.h>
#include <VulkanRenderDevice/BowVulkanRenderDevice.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/BowVulkanRenderSurface.h>
#include <VulkanRenderDevice/Device/BowVulkanSwapchain.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <unordered_map>

namespace bow
{

std::unordered_map<GLFWwindow *, VulkanGraphicsWindow *> g_Windows;

VulkanGraphicsWindow::VulkanGraphicsWindow() : m_renderDevice(nullptr), m_surface(nullptr), m_window(nullptr), m_Width(0), m_Height(0), m_guid(Utils::GenerateGUID()) { FN("VulkanGraphicsWindow::VulkanGraphicsWindow"); }

VulkanGraphicsWindow::~VulkanGraphicsWindow()
{
    FN("VulkanGraphicsWindow::~VulkanGraphicsWindow");

    VRelease();
}

bool VulkanGraphicsWindow::Initialize(VulkanRenderDevice *device, uint32_t width, uint32_t height, const std::string &title, WindowType windowType)
{
    FN("VulkanGraphicsWindow::Initialize");

    m_renderDevice = device;

    // Create a fullscrenn window?
    GLFWmonitor *monitor = nullptr;
    if (windowType == WindowType::Fullscreen)
    {
        LOG_TRACE("glfwGetPrimaryMonitor");
        monitor = glfwGetPrimaryMonitor();
    }

    // Creating Window with a cool custom deleter
    LOG_TRACE("glfwCreateWindow");
    m_window = glfwCreateWindow(width, height, (title + " (Vulkan)").c_str(), monitor, nullptr);
    if (!m_window)
    {
        LOG_ERROR("Error while creating Vulkan-Window with glfw!");
        return false;
    }

    if (g_Windows.find(m_window) == g_Windows.end())
    {
        g_Windows.insert(std::pair<GLFWwindow *, VulkanGraphicsWindow *>(m_window, this));
    }

    LOG_TRACE("glfwGetWindowSize");
    glfwGetWindowSize(m_window, &m_Width, &m_Height);

    LOG_TRACE("glfwSetWindowSizeCallback");
    glfwSetWindowSizeCallback(m_window, VulkanGraphicsWindow::ResizeCallback);

    return true;
}

void VulkanGraphicsWindow::VRelease()
{
    FN("VulkanGraphicsWindow::VRelease");

    if (m_surface.get() != nullptr)
    {
        m_surface.reset();
    }

    LOG_TRACE("glfwDestroyWindow");
    glfwDestroyWindow(m_window);
    if (g_Windows.find(m_window) != g_Windows.end())
    {
        g_Windows.erase(m_window);
    }
    m_window = nullptr;
}

void *VulkanGraphicsWindow::VGetHandle() const
{
    FN("VulkanGraphicsWindow::VGetHandle");

    return m_window;
}

RenderContextPtr VulkanGraphicsWindow::VGetContext() const
{
    FN("VulkanGraphicsWindow::VGetContext");

    return m_surface;
}

void VulkanGraphicsWindow::VPollWindowEvents() const
{
    FN("VulkanGraphicsWindow::VPollWindowEvents");
    OPTICK_EVENT();

    LOG_TRACE("glfwPollEvents");
    glfwPollEvents();
}

void VulkanGraphicsWindow::VSetWindowTitle(const char *title) const
{
    FN("VulkanGraphicsWindow::VSetWindowTitle");

    LOG_TRACE("glfwSetWindowTitle");
    glfwSetWindowTitle(m_window, title);
}

void VulkanGraphicsWindow::VHideCursor() const
{
    FN("VulkanGraphicsWindow::VHideCursor");
    OPTICK_EVENT();

    LOG_TRACE("glfwSetInputMode");
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void VulkanGraphicsWindow::VShowCursor() const
{
    FN("VulkanGraphicsWindow::VShowCursor");
    OPTICK_EVENT();

    LOG_TRACE("glfwSetInputMode");
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int VulkanGraphicsWindow::VGetWidth() const
{
    FN("VulkanGraphicsWindow::VGetWidth");

    return m_Width;
}

int VulkanGraphicsWindow::VGetHeight() const
{
    FN("VulkanGraphicsWindow::VGetHeight");

    return m_Height;
}

bool VulkanGraphicsWindow::VIsVisible() const
{
    FN("VulkanGraphicsWindow::VIsVisible");

    LOG_TRACE("glfwGetWindowAttrib");
    int visible = glfwGetWindowAttrib(m_window, GLFW_VISIBLE);
    return visible == 1;
}

bool VulkanGraphicsWindow::VIsFocused() const
{
    FN("VulkanGraphicsWindow::VIsFocused");

    LOG_TRACE("glfwGetWindowAttrib");
    int focused = glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
    return focused == 1;
}

bool VulkanGraphicsWindow::VShouldClose() const
{
    FN("VulkanGraphicsWindow::VShouldClose");
    OPTICK_EVENT();

    LOG_TRACE("glfwPollEvents");
    glfwPollEvents();
    LOG_TRACE("glfwWindowShouldClose");
    return glfwWindowShouldClose(m_window) != 0;
}

bool VulkanGraphicsWindow::InitializeSurface()
{
    FN("VulkanGraphicsWindow::InitializeSurface");

    m_surface = VulkanRenderSurfacePtr(new VulkanRenderSurface(m_window));
    if (!m_surface->InitializeSurface(m_renderDevice))
    {
        LOG_ERROR("Error while initializing render surface!");
        return false;
    }

    return true;
}

bool VulkanGraphicsWindow::InitializeSwapchain()
{
    FN("VulkanGraphicsWindow::InitializeSwapchain");

    if (m_surface.get() == nullptr)
    {
        LOG_ERROR("Error while initializing swapchain: Surface is not initialized!");
        return false;
    }

    int width, height;
    LOG_TRACE("glfwGetFramebufferSize");
    glfwGetFramebufferSize(m_window, &width, &height);
    if (!m_surface->InitializeSwapchain(std::max(width, 1), std::max(height, 1)))
    {
        LOG_ERROR("Error while initializing swapchain!");
        return false;
    }

    return true;
}

void VulkanGraphicsWindow::ResizeCallback(GLFWwindow *window, int width, int height)
{
    FN("VulkanGraphicsWindow::ResizeCallback");

    if (g_Windows.find(window) != g_Windows.end())
    {
        g_Windows[window]->m_Width = width;
        g_Windows[window]->m_Height = height;
    }
}

} // namespace bow
