#include "BowDIInputDevice.h"
#include "BowLogger.h"

#include "BowDIMouseDevice.h"
#include "BowDIKeyboardDevice.h"

namespace bow {
	
	DIInputDevice::DIInputDevice(void)
	{
		m_pDirectInput = nullptr;
	}

	DIInputDevice::~DIInputDevice(void)
	{
		VRelease();
	}

	bool DIInputDevice::Initialize()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL)))
		{
			return false;
		}
		return true;
	}

	void DIInputDevice::VRelease(void)
	{
		if (m_pDirectInput)
		{
			m_pDirectInput->Release();
			m_pDirectInput = nullptr;
		}
	}

	MousePtr DIInputDevice::VCreateMouseObject(GraphicsWindowPtr window)
	{
		DIMouseDevice *pDevice = new DIMouseDevice(m_pDirectInput, window->VGetHandle());
		pDevice->Initialize();
		return DIMousePtr(pDevice);
	}

	KeyboardPtr DIInputDevice::VCreateKeyboardObject(GraphicsWindowPtr window)
	{
		DIKeyboardDevice *pDevice = new DIKeyboardDevice(m_pDirectInput, window->VGetHandle());
		pDevice->Initialize();
		return DIKeyboardPtr(pDevice);
	}

}
