#include <DirectInputDevice/BowDIInputDevice.h>

#include <DirectInputDevice/BowDIKeyboardDevice.h>
#include <DirectInputDevice/BowDIMouseDevice.h>

#include <RenderDevice/Device/BowGraphicsWindow.h>



#include <optick.h>

#include <Windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace bow
{

DIInputDevice::DIInputDevice()
{
    FN("DIInputDevice::DIInputDevice");

    m_pDirectInput = nullptr;
}

DIInputDevice::~DIInputDevice()
{
    FN("DIInputDevice::~DIInputDevice");

    VRelease();
}

bool DIInputDevice::Initialize()
{
    FN("DIInputDevice::Initialize");

    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_pDirectInput, NULL)))
    {
        return false;
    }
    return true;
}

void DIInputDevice::VRelease()
{
    FN("DIInputDevice::VRelease");

    if (m_pDirectInput)
    {
        m_pDirectInput->Release();
        m_pDirectInput = nullptr;
    }
}

MousePtr DIInputDevice::VCreateMouseObject(GraphicsWindowPtr window)
{
    FN("DIInputDevice::VCreateMouseObject");
    OPTICK_EVENT();

    GLFWwindow *glfwWindow = static_cast<GLFWwindow *>(window->VGetHandle());
    if (glfwWindow == nullptr)
    {
        return DIMousePtr(nullptr);
    }

    DIMouseDevice *pDevice = new DIMouseDevice(m_pDirectInput, glfwGetWin32Window(glfwWindow));
    pDevice->Initialize();
    return DIMousePtr(pDevice);
}

KeyboardPtr DIInputDevice::VCreateKeyboardObject(GraphicsWindowPtr window)
{
    FN("DIInputDevice::VCreateKeyboardObject");
    OPTICK_EVENT();

    GLFWwindow *glfwWindow = static_cast<GLFWwindow *>(window->VGetHandle());
    if (glfwWindow == nullptr)
    {
        return DIKeyboardPtr(nullptr);
    }

    DIKeyboardDevice *pDevice = new DIKeyboardDevice(m_pDirectInput, glfwGetWin32Window(glfwWindow));
    pDevice->Initialize();
    return DIKeyboardPtr(pDevice);
}

} // namespace bow
