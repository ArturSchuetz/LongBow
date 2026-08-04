#include <NativeInputDevice/BowNativeInputDevice.h>

#include <NativeInputDevice/BowNativeKeyboardDevice.h>
#include <NativeInputDevice/BowNativeMouseDevice.h>

#include <RenderDevice/Device/BowGraphicsWindow.h>



#include <optick.h>

#include <GLFW/glfw3.h>

namespace bow
{

NativeInputDevice::NativeInputDevice() { FN("NativeInputDevice::NativeInputDevice"); }

NativeInputDevice::~NativeInputDevice()
{
    FN("NativeInputDevice::~NativeInputDevice");

    VRelease();
}

bool NativeInputDevice::Initialize()
{
    FN("NativeInputDevice::Initialize");

    return true;
}

void NativeInputDevice::VRelease() { FN("NativeInputDevice::VRelease"); }

MousePtr NativeInputDevice::VCreateMouseObject(GraphicsWindowPtr window)
{
    FN("NativeInputDevice::VCreateMouseObject");
    OPTICK_EVENT();

    GLFWwindow *glfwWindow = static_cast<GLFWwindow *>(window->VGetHandle());
    if (glfwWindow == nullptr)
    {
        return NativeMousePtr(nullptr);
    }

    NativeMouseDevice *pDevice = new NativeMouseDevice(glfwWindow);
    pDevice->Initialize();
    return NativeMousePtr(pDevice);
}

KeyboardPtr NativeInputDevice::VCreateKeyboardObject(GraphicsWindowPtr window)
{
    FN("NativeInputDevice::VCreateKeyboardObject");
    OPTICK_EVENT();

    GLFWwindow *glfwWindow = static_cast<GLFWwindow *>(window->VGetHandle());
    if (glfwWindow == nullptr)
    {
        return NativeKeyboardPtr(nullptr);
    }

    NativeKeyboardDevice *pDevice = new NativeKeyboardDevice(glfwWindow);
    pDevice->Initialize();
    return NativeKeyboardPtr(pDevice);
}

} // namespace bow
