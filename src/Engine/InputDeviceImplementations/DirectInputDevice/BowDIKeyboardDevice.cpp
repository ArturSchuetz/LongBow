#include "BowDIKeyboardDevice.h"
#include "BowLogger.h"

#include "BowDITypeConverter.h"

namespace bow {
	
	DIKeyboardDevice::DIKeyboardDevice(IDirectInput8 *directInput, HWND windowHandle) 
		: DIInputDeviceBase(directInput, windowHandle)
	{
	}

	DIKeyboardDevice::~DIKeyboardDevice()
	{
	}

	bool DIKeyboardDevice::Initialize(void)
	{
		if (!DIInputDeviceBase::Initialize(GUID_SysKeyboard, &c_dfDIKeyboard))
		{
			return false;
		}

		memset(m_keys, 0, sizeof(m_keys));

		m_pDirectInputDevice->Acquire();
		return true;
	}

	bool DIKeyboardDevice::VUpdate(void)
	{
		return DIInputDeviceBase::GetData(InputDeviceType::Keyboard, &m_keys[0], NULL);
	}

	bool DIKeyboardDevice::VIsPressed(Key keyID) const
	{
		if (m_keys[DITypeConverter::To(keyID)] & 0x80)
		{
			return true;
		}
		return false;
	}

}
