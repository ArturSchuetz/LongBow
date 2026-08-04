
#include <OpenGL3xRenderDevice/Device/BowOGL3xGraphicsWindow.h>

#include <OpenGL3xRenderDevice/BowOGL3xRenderDevice.h>
#include <OpenGL3xRenderDevice/Device/BowOGL3xRenderContext.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

#include <unordered_map>

namespace bow
{

std::unordered_map<GLFWwindow *, OGLGraphicsWindow *> g_Windows;

OGLGraphicsWindow::OGLGraphicsWindow() : m_ParentDevice(nullptr), m_Context(nullptr), m_Window(nullptr), m_Width(0), m_Height(0) { FN("OGLGraphicsWindow::OGLGraphicsWindow"); }

bool OGLGraphicsWindow::Initialize(uint32_t width, uint32_t height, const std::string &title, WindowType windowType, OGLRenderDevice *device)
{
    FN("OGLGraphicsWindow::Initialize");

    // Create a fullscrenn window?
    GLFWmonitor *monitor = nullptr;
    if (windowType == WindowType::Fullscreen)
    {
        LOG_TRACE("glfwGetPrimaryMonitor");
        monitor = glfwGetPrimaryMonitor();
    }

    // Headless Rendering
    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // Ask for a core profile explicitly.
    //
    // Without these hints GLFW hands back whatever the driver happens to
    // default to, which on most desktop drivers is a compatibility profile.
    // That silently kept removed entry points such as glBegin working and left
    // the backend's actual version requirement undefined. 4.5 is the floor
    // because direct state access, used throughout the buffer and texture
    // code, became core there.
    LOG_TRACE("glfwWindowHint");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#ifdef _DEBUG
    // A debug context is what makes glDebugMessageCallback deliver anything.
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    // Creating Window with a cool custom deleter
    LOG_TRACE("glfwCreateWindow");
    m_Window = glfwCreateWindow(width, height, (title + " (OpenGL)").c_str(), monitor, NULL);

    if (!m_Window)
    {
        // Fall back to 3.3 core so the backend still comes up on hardware or
        // drivers that stop short of 4.5. Direct state access is then absent
        // and the affected paths report it rather than crashing.
        LOG_WARNING("No OpenGL 4.5 core context available, retrying with 3.3 core.");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        LOG_TRACE("glfwCreateWindow");
        m_Window = glfwCreateWindow(width, height, (title + " (OpenGL)").c_str(), monitor, NULL);
    }

    if (g_Windows.find(m_Window) == g_Windows.end())
    {
        g_Windows.insert(std::pair<GLFWwindow *, OGLGraphicsWindow *>(m_Window, this));
    }

    if (!m_Window)
    {
        LOG_TRACE("glfwTerminate");
        glfwTerminate();
        LOG_ERROR("Error while creating OpenGL-Window with glfw!");
        return false;
    }

    LOG_TRACE("glfwGetWindowSize");
    glfwGetWindowSize(m_Window, &m_Width, &m_Height);

    LOG_TRACE("glfwSetWindowSizeCallback");
    glfwSetWindowSizeCallback(m_Window, OGLGraphicsWindow::ResizeCallback);

    m_ParentDevice = device;
    m_Context = OGLRenderContextPtr(new OGLRenderContext(m_Window));

    LOG_TRACE("OpenGL-Window sucessfully initialized!");
    return m_Context->Initialize(m_ParentDevice);
}

OGLGraphicsWindow::~OGLGraphicsWindow()
{
    FN("OGLGraphicsWindow::~OGLGraphicsWindow");
    VRelease();
}

void OGLGraphicsWindow::VRelease()
{
    FN("OGLGraphicsWindow::VRelease");

    if (m_Context.get() != nullptr)
    {
        m_Context->VRelease();
        m_Context.reset();
    }
    LOG_TRACE("glfwDestroyWindow");
    glfwDestroyWindow(m_Window);
    if (g_Windows.find(m_Window) != g_Windows.end())
    {
        g_Windows.erase(m_Window);
    }
    m_Window = nullptr;

    LOG_TRACE("OGLGraphicsWindow released");
}

void *OGLGraphicsWindow::VGetHandle() const
{
    FN("OGLGraphicsWindow::VGetHandle");

    return m_Window;
}

RenderContextPtr OGLGraphicsWindow::VGetContext() const
{
    FN("OGLGraphicsWindow::VGetContext");

    return m_Context;
}

void OGLGraphicsWindow::VPollWindowEvents() const
{
    FN("OGLGraphicsWindow::VPollWindowEvents");
    OPTICK_EVENT();

    LOG_TRACE("glfwPollEvents");
    glfwPollEvents();
}

void OGLGraphicsWindow::VSetWindowTitle(const char *title) const
{
    FN("OGLGraphicsWindow::VSetWindowTitle");

    LOG_TRACE("glfwSetWindowTitle");
    glfwSetWindowTitle(m_Window, title);
}

void OGLGraphicsWindow::VHideCursor() const
{
    FN("OGLGraphicsWindow::VHideCursor");
    OPTICK_EVENT();

    LOG_TRACE("glfwSetInputMode");
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void OGLGraphicsWindow::VShowCursor() const
{
    FN("OGLGraphicsWindow::VShowCursor");
    OPTICK_EVENT();

    LOG_TRACE("glfwSetInputMode");
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int OGLGraphicsWindow::VGetWidth() const
{
    FN("OGLGraphicsWindow::VGetWidth");

    return m_Width;
}

int OGLGraphicsWindow::VGetHeight() const
{
    FN("OGLGraphicsWindow::VGetHeight");

    return m_Height;
}

bool OGLGraphicsWindow::VIsVisible() const
{
    FN("OGLGraphicsWindow::VIsVisible");

    LOG_TRACE("glfwGetWindowAttrib");
    int visible = glfwGetWindowAttrib(m_Window, GLFW_VISIBLE);
    return visible == 1;
}

bool OGLGraphicsWindow::VIsFocused() const
{
    FN("OGLGraphicsWindow::VIsFocused");

    LOG_TRACE("glfwGetWindowAttrib");
    int focused = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
    return focused == 1;
}

bool OGLGraphicsWindow::VShouldClose() const
{
    FN("OGLGraphicsWindow::VShouldClose");
    OPTICK_EVENT();

    LOG_TRACE("glfwPollEvents");
    glfwPollEvents();
    return glfwWindowShouldClose(m_Window) != 0;
}

void OGLGraphicsWindow::ResizeCallback(GLFWwindow *window, int width, int height)
{
    FN("OGLGraphicsWindow::ResizeCallback");

    if (g_Windows.find(window) != g_Windows.end())
    {
        g_Windows[window]->m_Width = width;
        g_Windows[window]->m_Height = height;
    }
}
} // namespace bow
