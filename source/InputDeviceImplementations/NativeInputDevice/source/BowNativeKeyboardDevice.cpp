#include <NativeInputDevice/BowNativeKeyboardDevice.h>

#include <NativeInputDevice/BowNativeTypeConverter.h>

#include <optick.h>

#include <GLFW/glfw3.h>

namespace bow
{

NativeKeyboardDevice::NativeKeyboardDevice(GLFWwindow *glfwWindow) : m_glfwWindow(glfwWindow), m_keys() { FN("NativeKeyboardDevice::NativeKeyboardDevice"); }

NativeKeyboardDevice::~NativeKeyboardDevice() { FN("NativeKeyboardDevice::~NativeKeyboardDevice"); }

bool NativeKeyboardDevice::Initialize()
{
    FN("NativeKeyboardDevice::Initialize");

    return true;
}

bool NativeKeyboardDevice::VUpdate()
{
    FN("NativeKeyboardDevice::VUpdate");
    OPTICK_EVENT();

    return true;
}

bool NativeKeyboardDevice::VIsPressed(Key keyID) const
{
    FN("NativeKeyboardDevice::VIsPressed");
    OPTICK_EVENT();

    LOG_TRACE("glfwGetKey");
    int state = glfwGetKey(m_glfwWindow, NativeTypeConverter::To(keyID));
    return state == GLFW_PRESS;
}

} // namespace bow
