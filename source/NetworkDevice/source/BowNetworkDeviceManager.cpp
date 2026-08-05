#include <NetworkDevice/BowNetworkDeviceManager.h>
#include <CoreSystems/BowLogger.h>

#include <string>

#include <NetworkDevice/IBowNetworkDevice.h>

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
		if (Device != nullptr)
		{
			return Device;
		}

		// The backend builds as WinSockNetworkDevice, and this used to look for
		// a NetworkDevice.dll that nothing produced -- which is why networking
		// could never have loaded in this engine.
#ifdef _DEBUG
		const char *libraryName = "WinSockNetworkDeviced.dll";
#else
		const char *libraryName = "WinSockNetworkDevice.dll";
#endif

		HMODULE hDLL = LoadLibrary(libraryName);
		if (!hDLL)
		{
			LOG_ERROR("Could not load %s (error code %s).", libraryName, std::to_string(GetLastError()).c_str());
			return NetworkDevicePtr(nullptr);
		}

		CREATENETWORKDEVICE createNetworkDevice = (CREATENETWORKDEVICE)GetProcAddress(hDLL, "CreateNetworkDevice");
		if (createNetworkDevice == nullptr)
		{
			LOG_ERROR("%s does not export CreateNetworkDevice.", libraryName);
			return NetworkDevicePtr(nullptr);
		}

		INetworkDevice *pDevice = createNetworkDevice(EventLogger::GetInstance());
		if (pDevice == nullptr)
		{
			LOG_ERROR("Could not create a network device from %s!", libraryName);
			return NetworkDevicePtr(nullptr);
		}

		Device = NetworkDevicePtr(pDevice);
		return Device;
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
