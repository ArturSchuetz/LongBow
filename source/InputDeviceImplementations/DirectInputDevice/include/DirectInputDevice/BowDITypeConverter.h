#pragma once
#include "DirectInputDevice/DirectInputDevice_api.h"

#include "InputDevice/IBowKeyboard.h"
#include "InputDevice/IBowMouse.h"

#include <dinput.h>

namespace bow
{

class DITypeConverter
{
  public:
    static uint32_t To(Key key);
    static uint32_t To(MouseButton key);
};

} // namespace bow
