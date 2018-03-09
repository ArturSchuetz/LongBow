#include "BowNetworkDeviceManager.h"
#include "BowLogger.h"

#include "IBowNetworkDevice.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow {

	static NetworkDevicePtr Device;

	// Function inside the DLL we want to call to create our Device-Object
	extern "C"
	{
		typedef INetworkDevice* (*CREATENETWORKDEVICE)(EventLogger& logger);
	}

	NetworkDeviceManager::~NetworkDeviceManager(void)
	{
		while (Device != nullptr)
		{
			Device = nullptr;
		}
	}

	NetworkDeviceManager& NetworkDeviceManager::GetInstance()
	{
		static NetworkDeviceManager instance;
		return instance;
	}

	NetworkDevicePtr NetworkDeviceManager::GetOrCreateDevice()
	{
		if (Device == nullptr)
		{
			HMODULE hDLL = NULL;

#ifdef _DEBUG
			hDLL = LoadLibraryExW(L"NetworkDevice_d.dll", NULL, 0);
#else
			hDLL = LoadLibraryExW(L"NetworkDevice.dll", NULL, 0);
#endif
			if (!hDLL)
			{
#ifdef _DEBUG
				LOG_ERROR("Could not find NetworkDevice_d.dll!");
#else
				LOG_ERROR("Could not find NetworkDevice.dll!");
#endif
				return NetworkDevicePtr(nullptr);
			}

			CREATENETWORKDEVICE _CreateNetworkDevice = 0;

			// Zeiger auf die dll Funktion 'CreateRenderDevice'
			_CreateNetworkDevice = (CREATENETWORKDEVICE)GetProcAddress(hDLL, "CreateNetworkDevice");
			INetworkDevice *pDevice = _CreateNetworkDevice(EventLogger::GetInstance());

			// aufruf der dll Create-Funktionc
			if (pDevice == nullptr)
			{
				LOG_ERROR("Could not create Network Device from DLL!");
				return NetworkDevicePtr(nullptr);
			}
			else
			{
				Device = NetworkDevicePtr(pDevice);
				return Device;
			}
		}
		else
		{
			return Device;
		}
	}

	void NetworkDeviceManager::ReleaseDevice()
	{
		if (Device != nullptr)
		{
			Device->VRelease();
			Device = nullptr;
		}
	}
}
