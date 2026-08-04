#pragma once
#include "DirectInputDevice/DirectInputDevice_api.h"

#include "InputDevice/IBowInputDevice.h"

#include <dinput.h>

namespace bow
{

class DIInputDevice : public IInputDevice
{
  public:
    DIInputDevice();
    ~DIInputDevice();

    bool Initialize();
    void VRelease() sealed;

    MousePtr VCreateMouseObject(GraphicsWindowPtr window);
    KeyboardPtr VCreateKeyboardObject(GraphicsWindowPtr window);

  private:
    // you shall not copy
    DIInputDevice(const DIInputDevice &obj) = delete;
    DIInputDevice &operator=(const DIInputDevice &obj) = delete;

    IDirectInput8 *m_pDirectInput;
};

} // namespace bow
