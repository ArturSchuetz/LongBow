#pragma once
#include "InputDevice/BowInputPredeclares.h"
#include "InputDevice/InputDevice_api.h"

#include "RenderDevice/Device/BowGraphicsWindow.h"

namespace bow
{

enum class InputDeviceAPI : char
{
    NativeInput,
    DirectInput
};

//! \brief InputDeviceManager is a singleton and creates input devices.
class INPUTDEVICE_API InputDeviceManager
{
  public:
    ~InputDeviceManager();

    static InputDeviceManager &GetInstance();

    KeyboardPtr CreateKeyboardObject(GraphicsWindowPtr window);
    MousePtr CreateMouseObject(GraphicsWindowPtr window);

  protected:
    InputDeviceManager() {}

  private:
    void ReleaseDevice(InputDeviceAPI api);
    InputDevicePtr GetOrCreateDevice(InputDeviceAPI api);

    InputDeviceManager(const InputDeviceManager &) {}; //!< You shall not copy
    InputDeviceManager &operator=(const InputDeviceManager &) { return *this; }
};
} // namespace bow
