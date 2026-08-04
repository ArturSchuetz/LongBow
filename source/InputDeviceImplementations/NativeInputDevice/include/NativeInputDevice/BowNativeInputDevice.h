#pragma once
#include <NativeInputDevice/NativeInputDevice_api.h>

#include <InputDevice/IBowInputDevice.h>

namespace bow
{

class NativeInputDevice : public IInputDevice
{
  public:
    NativeInputDevice();
    ~NativeInputDevice();

    bool Initialize();
    void VRelease() sealed;

    MousePtr VCreateMouseObject(GraphicsWindowPtr window);
    KeyboardPtr VCreateKeyboardObject(GraphicsWindowPtr window);

  private:
    // you shall not copy
    NativeInputDevice(const NativeInputDevice &obj) = delete;
    NativeInputDevice &operator=(const NativeInputDevice &obj) = delete;
};

} // namespace bow
