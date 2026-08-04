#include <DirectInputDevice/BowDIKeyboardDevice.h>
#include <DirectInputDevice/BowDITypeConverter.h>

#include <optick.h>

namespace bow
{

DIKeyboardDevice::DIKeyboardDevice(IDirectInput8 *directInput, HWND windowHandle) : DIInputDeviceBase(directInput, windowHandle), m_keys()
{
    FN("DIKeyboardDevice::DIKeyboardDevice");
    memset(m_keys, 0, sizeof(m_keys));
}

DIKeyboardDevice::~DIKeyboardDevice() { FN("DIKeyboardDevice::~DIKeyboardDevice"); }

bool DIKeyboardDevice::Initialize()
{
    FN("DIMouseDevice::Initialize");

    if (!DIInputDeviceBase::Initialize(GUID_SysKeyboard, &c_dfDIKeyboard))
    {
        return false;
    }

    m_pDirectInputDevice->Acquire();
    return true;
}

bool DIKeyboardDevice::VUpdate()
{
    FN("DIMouseDevice::VUpdate");
    OPTICK_EVENT();

    return DIInputDeviceBase::GetData(InputDeviceType::Keyboard, &m_keys[0], NULL);
}

bool DIKeyboardDevice::VIsPressed(Key keyID) const
{
    FN("DIMouseDevice::VIsPressed");
    OPTICK_EVENT();

    if (m_keys[DITypeConverter::To(keyID)] & 0x80)
    {
        return true;
    }
    return false;
}

} // namespace bow
