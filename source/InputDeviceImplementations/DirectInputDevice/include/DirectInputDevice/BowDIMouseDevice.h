#pragma once
#include "DirectInputDevice/BowDIInputDeviceBase.h"
#include "DirectInputDevice/DirectInputDevice_api.h"

#include "InputDevice/IBowMouse.h"

#include <dinput.h>

namespace bow
{

class DIMouseDevice : public DIInputDeviceBase, public IMouse
{
  public:
    DIMouseDevice(IDirectInput8 *directInput, HWND windowHandle);
    ~DIMouseDevice();

    bool Initialize();
    bool VUpdate();
    bool VIsPressed(MouseButton keyID) const;

    Vector3<long> VGetRelativePosition() const;
    Vector3<long> VGetAbsolutePosition() const;
    Vector3<long> VGetAbsolutePositionInsideWindow() const;

    void VHideCursor();
    void VShowCursor();

    bool VSetCursorPosition(int x, int y);

    // Direct Input supports this with exclusive mode
    void VCageMouse(bool cage);
    void VSetCage(long left, long top, long right, long bottom);

  private:
    // you shall not copy
    DIMouseDevice(const DIMouseDevice &obj) = delete;
    DIMouseDevice &operator=(const DIMouseDevice &obj) = delete;

    HANDLE m_Event;
    bool m_Pressed[8];

    long m_x;
    long m_y;

    long m_relativeX;
    long m_relativeY;
    long m_relativeScrollWheel;

    bool m_shouldCage;
    RECT m_cageBounds;
};

typedef std::shared_ptr<DIMouseDevice> DIMousePtr;

} // namespace bow
