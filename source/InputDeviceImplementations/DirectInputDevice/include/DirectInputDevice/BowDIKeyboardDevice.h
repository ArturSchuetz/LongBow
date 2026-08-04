#pragma once
#include "DirectInputDevice/BowDIInputDeviceBase.h"
#include "DirectInputDevice/DirectInputDevice_api.h"

#include "InputDevice/IBowKeyboard.h"

#include <dinput.h>

namespace bow
{

class DIKeyboardDevice : public DIInputDeviceBase, public IKeyboard
{
  public:
    DIKeyboardDevice(IDirectInput8 *directInput, HWND windowHandle);
    ~DIKeyboardDevice();

    bool Initialize();
    bool VUpdate();
    bool VIsPressed(Key keyID) const;

  protected:
    char m_keys[256];

  private:
    // you shall not copy
    DIKeyboardDevice(const DIKeyboardDevice &obj) = delete;
    DIKeyboardDevice &operator=(const DIKeyboardDevice &obj) = delete;
};

typedef std::shared_ptr<DIKeyboardDevice> DIKeyboardPtr;
} // namespace bow
