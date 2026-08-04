#pragma once
#include "InputDevice/InputDevice_api.h"

#include <memory>

namespace bow
{

class INPUTDEVICE_API InputDeviceManager;
enum class InputDeviceAPI : char;

class IInputDevice;
typedef std::shared_ptr<IInputDevice> InputDevicePtr;

class IKeyboard;
typedef std::shared_ptr<IKeyboard> KeyboardPtr;

class IMouse;
typedef std::shared_ptr<IMouse> MousePtr;
} // namespace bow
