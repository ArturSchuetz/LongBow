#include "BowInputDeviceManager.h"
#include "BowLogger.h"

#include "IBowInputDevice.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Bow
{
	namespace Input
	{
		static std::unordered_map<API, InputDevicePtr> DeviceMap;

		// Function inside the DLL we want to call to create our Device-Object
		extern "C"
		{
			typedef IInputDevice* (*CREATEINPUTDEVICE)(Core::EventLogger& logger);
		}

		InputDeviceManager::~InputDeviceManager(void)
		{
			while (DeviceMap.begin() != DeviceMap.end())
			{
				ReleaseDevice(DeviceMap.begin()->first);
			}
			DeviceMap.clear();
		}

		void InputDeviceManager::ReleaseDevice(API api)
		{
			if (DeviceMap.find(api) != DeviceMap.end())
			{
				if (DeviceMap[api].get() != nullptr)
				{
					DeviceMap[api]->VRelease();
					DeviceMap.erase(api);
				}
			}
		}

		InputDeviceManager& InputDeviceManager::GetInstance()
		{
			static InputDeviceManager instance;
			return instance;
		}

		InputDevicePtr InputDeviceManager::GetOrCreateDevice(API api)
		{
			if (DeviceMap.find(api) == DeviceMap.end())
			{
				HMODULE hDLL = NULL;
				switch (api)
				{
				case API::DirectInput:
					{
#ifdef _DEBUG
						hDLL = LoadLibraryExW(L"DirectInputDevice_d.dll", NULL, 0);
#else
						hDLL = LoadLibraryExW(L"DirectInputDevice.dll", NULL, 0);
#endif
						if (!hDLL)
						{
							LOG_ERROR("Could not find DirectInputDevice.dll!");
							return InputDevicePtr(nullptr);
						}
					}
					break;

				default:
					{
						LOG_ERROR("Input API is not supported!");
						return InputDevicePtr(nullptr);
					}
					break;
				}

				CREATEINPUTDEVICE _CreateInputDevice = 0;

				// Zeiger auf die dll Funktion 'CreateRenderDevice'
				_CreateInputDevice = (CREATEINPUTDEVICE)GetProcAddress(hDLL, "CreateInputDevice");
				IInputDevice *pDevice = _CreateInputDevice(Core::EventLogger::GetInstance());

				// aufruf der dll Create-Funktionc
				if (pDevice == nullptr)
				{
					LOG_ERROR("Could not create Input Device from DLL!");
					return InputDevicePtr(nullptr);
				}
				else
				{
					DeviceMap.insert(std::pair<API, InputDevicePtr>(api, InputDevicePtr(pDevice)));
					return DeviceMap[api];
				}
			}
			else
			{
				return DeviceMap[api];
			}
		}

		KeyboardPtr InputDeviceManager::CreateKeyboardObject(Renderer::GraphicsWindowPtr window)
		{
			InputDevicePtr pDevice = GetOrCreateDevice(API::DirectInput);
			return pDevice->VCreateKeyboardObject(window);
		}

		MousePtr InputDeviceManager::CreateMouseObject(Renderer::GraphicsWindowPtr window)
		{
			InputDevicePtr pDevice = GetOrCreateDevice(API::DirectInput);
			return pDevice->VCreateMouseObject(window);
		}

	}
}