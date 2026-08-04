
#include <DirectInputDevice/BowDIMouseDevice.h>
#include <DirectInputDevice/BowDITypeConverter.h>

#include <optick.h>

#define BUFFER_SIZE 16

namespace bow
{

DIMouseDevice::DIMouseDevice(IDirectInput8 *directInput, HWND windowHandle)
    : DIInputDeviceBase(directInput, windowHandle), m_Event(nullptr), m_x(0), m_y(0), m_relativeX(0), m_relativeY(0), m_relativeScrollWheel(0), m_shouldCage(false), m_cageBounds({0, 0, 0, 0})
{
    FN("DIMouseDevice::DIMouseDevice");
    m_Event = nullptr;
    memset(m_Pressed, false, sizeof(m_Pressed));

    m_x = 0;
    m_y = 0;

    m_relativeX = 0;
    m_relativeY = 0;

    m_shouldCage = false;
    m_cageBounds.left = m_cageBounds.top = 0;
    m_cageBounds.right = m_cageBounds.bottom = 999999;
}

DIMouseDevice::~DIMouseDevice() { FN("DIMouseDevice::~DIMouseDevice"); }

bool DIMouseDevice::Initialize()
{
    FN("DIMouseDevice::Initialize");

    memset(m_Pressed, false, sizeof(m_Pressed));
    m_x = m_y = 0;

    if (!DIInputDeviceBase::Initialize(GUID_SysMouse, &c_dfDIMouse))
    {
        return false;
    }

    if (!(m_Event = CreateEvent(NULL, FALSE, FALSE, NULL)))
    {
        return false;
    }

    if (FAILED(m_pDirectInputDevice->SetEventNotification(m_Event)))
    {
        return false;
    }

    // Buffer
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = BUFFER_SIZE;

    if (FAILED(m_pDirectInputDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
    {
        return false;
    }

    m_pDirectInputDevice->Acquire();
    return true;
}

bool DIMouseDevice::VUpdate()
{
    FN("DIMouseDevice::VUpdate");
    OPTICK_EVENT();

    DIDEVICEOBJECTDATA od[BUFFER_SIZE];
    DWORD dwNumElem = BUFFER_SIZE;

    memset(&od, 0, sizeof(od));

    if (FAILED(GetData(InputDeviceType::Mouse, &od[0], &dwNumElem)))
    {
        return false;
    }

    POINT point;
    if (GetCursorPos(&point))
    {
        m_x = point.x;
        m_y = point.y;

        if (m_shouldCage)
        {
            if (m_x < m_cageBounds.left)
            {
                m_x = m_cageBounds.left;
            }
            else if (m_x > m_cageBounds.right)
            {
                m_x = m_cageBounds.right;
            }

            if (m_y < m_cageBounds.top)
            {
                m_y = m_cageBounds.top;
            }
            else if (m_y > m_cageBounds.bottom)
            {
                m_y = m_cageBounds.bottom;
            }

            SetCursorPos(m_x, m_y);
        }
    }
    m_relativeX = m_relativeY = m_relativeScrollWheel = 0;
    for (DWORD i = 0; i < dwNumElem; i++)
    {
        switch (od[i].dwOfs)
        {
        case DIMOFS_X:
            m_relativeX += od[i].dwData;
            break;

        case DIMOFS_Y:
            m_relativeY += od[i].dwData;
            break;

        case DIMOFS_Z:
            m_relativeScrollWheel += od[i].dwData;
            break;

        case DIMOFS_BUTTON0:
            m_Pressed[0] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON1:
            m_Pressed[1] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON2:
            m_Pressed[2] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON3:
            m_Pressed[3] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON4:
            m_Pressed[4] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON5:
            m_Pressed[5] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON6:
            m_Pressed[6] = (od[i].dwData & 0x80) ? true : false;
            break;

        case DIMOFS_BUTTON7:
            m_Pressed[7] = (od[i].dwData & 0x80) ? true : false;
            break;
        }
    }

    return true;
}

bool DIMouseDevice::VIsPressed(MouseButton btnID) const
{
    FN("DIMouseDevice::VIsPressed");
    OPTICK_EVENT();

    return m_Pressed[DITypeConverter::To(btnID)];
}

Vector3<long> DIMouseDevice::VGetRelativePosition() const
{
    FN("DIMouseDevice::VGetRelativePosition");
    OPTICK_EVENT();

    return Vector3<long>(m_relativeX, m_relativeY, m_relativeScrollWheel);
}

Vector3<long> DIMouseDevice::VGetAbsolutePosition() const
{
    FN("DIMouseDevice::VGetAbsolutePosition");
    OPTICK_EVENT();

    return Vector3<long>(m_x, m_y, 0);
}

Vector3<long> DIMouseDevice::VGetAbsolutePositionInsideWindow() const
{
    FN("DIMouseDevice::VGetAbsolutePositionInsideWindow");
    OPTICK_EVENT();

    POINT point;
    if (GetCursorPos(&point))
    {
        if (ScreenToClient(m_windowHandle, &point))
        {
            return Vector3<long>(point.x, point.y, 0);
        }
    }

    return Vector3<long>(0, 0, 0);
}

void DIMouseDevice::VHideCursor()
{
    FN("DIMouseDevice::VHideCursor");
    OPTICK_EVENT();

    return;
}

void DIMouseDevice::VShowCursor()
{
    FN("DIMouseDevice::VShowCursor");
    OPTICK_EVENT();

    return;
}

bool DIMouseDevice::VSetCursorPosition(int x, int y)
{
    FN("DIMouseDevice::VSetCursorPosition");
    OPTICK_EVENT();

    if (SetCursorPos(x, y))
    {
        m_x = x;
        m_y = y;
        return true;
    }
    return false;
}

void DIMouseDevice::VCageMouse(bool cage)
{
    FN("DIMouseDevice::VCageMouse");
    OPTICK_EVENT();

    m_shouldCage = cage;
}

void DIMouseDevice::VSetCage(long left, long top, long right, long bottom)
{
    FN("DIMouseDevice::VSetCage");
    OPTICK_EVENT();

    m_cageBounds.left = left;
    m_cageBounds.top = top;
    m_cageBounds.right = right;
    m_cageBounds.bottom = bottom;
}

} // namespace bow
