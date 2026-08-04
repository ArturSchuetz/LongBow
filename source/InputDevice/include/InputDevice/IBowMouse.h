#pragma once
#include "InputDevice/BowInputPredeclares.h"
#include "InputDevice/InputDevice_api.h"

#include "CoreSystems/BowMath.h"

namespace bow
{

enum class MouseButton : unsigned char
{
    MOFS_BUTTON0,
    MOFS_BUTTON1,
    MOFS_BUTTON2,
    MOFS_BUTTON3,
    MOFS_BUTTON4,
    MOFS_BUTTON5,
    MOFS_BUTTON6,
    MOFS_BUTTON7,
    MOFS_BUTTON_LEFT,
    MOFS_BUTTON_RIGHT,
    MOFS_BUTTON_MIDDLE
};

class IMouse
{
  public:
    IMouse() {};
    virtual ~IMouse() {}

    virtual bool VUpdate() = 0;
    virtual bool VIsPressed(MouseButton keyID) const = 0;

    virtual Vector3<long> VGetRelativePosition() const = 0; // X and X are relative screen coordinates since last poll
                                                            // and Z represents the scroll wheel
    virtual Vector3<long> VGetAbsolutePosition() const = 0;
    virtual Vector3<long> VGetAbsolutePositionInsideWindow() const = 0;
    virtual bool VSetCursorPosition(int x, int y) = 0;

    virtual void VHideCursor() = 0;
    virtual void VShowCursor() = 0;

    virtual void VCageMouse(bool cage) = 0;
    virtual void VSetCage(long left, long top, long right, long bottom) = 0;
};

} // namespace bow
