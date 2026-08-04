#pragma once
#include <NativeInputDevice/NativeInputDevice_api.h>

#include <InputDevice/IBowKeyboard.h>
#include <InputDevice/IBowMouse.h>

#include <GLFW/glfw3.h>

namespace bow
{

class NativeTypeConverter
{
  public:
    static uint32_t To(Key key);
    static uint32_t To(MouseButton key);
};

} // namespace bow
