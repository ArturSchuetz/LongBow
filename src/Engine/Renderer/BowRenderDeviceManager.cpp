#include "BowRenderDeviceManager.h"
#include "BowLogger.h"

#include "IBowRenderDevice.h"

namespace Bow
{
	namespace Renderer
	{
		using namespace Core;

		static std::hash_map<API, RenderDevicePtr> DeviceMap;

		// Function inside the DLL we want to call to create our Device-Object
		extern "C"
		{
			typedef IRenderDevice* (*CREATERENDERDEVICE)(HINSTANCE hInstance, EventLogger& logger);
		}

		RenderDeviceManager::~RenderDeviceManager(void)
		{
			while (DeviceMap.begin() != DeviceMap.end())
			{
				ReleaseDevice(DeviceMap.begin()->first);
			}
			DeviceMap.clear();
		}


		RenderDeviceManager& RenderDeviceManager::GetInstance()
		{
			static RenderDeviceManager instance;
			return instance;
		}


		void RenderDeviceManager::ReleaseDevice(API api)
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

		RenderDevicePtr RenderDeviceManager::GetOrCreateDevice(API api)
		{
			return GetOrCreateDevice(api, GetModuleHandle(0));
		}


		RenderDevicePtr RenderDeviceManager::GetOrCreateDevice(API api, HINSTANCE hInstance)
		{
			if (DeviceMap.find(api) == DeviceMap.end())
			{
				HMODULE hDLL = NULL;
				switch (api)
				{
				case API::OpenGL3x:
				{
#ifdef _DEBUG
					hDLL = LoadLibraryExW(L"OGLRenderDevice_d.dll", NULL, 0);
#else
					hDLL = LoadLibraryExW(L"OGLRenderDevice.dll", NULL, 0);
#endif
					if (!hDLL)
					{
						LOG_ERROR("Could not find OGLRenderDevice.dll!");
						MessageBox(NULL, "Loading 'OGLRenderDevice.dll' failed.", "LongBowEngine - error", MB_OK | MB_ICONERROR);
						return RenderDevicePtr(nullptr);
					}
				}
					break;
				case API::Direct3D11:
				{
#ifdef _DEBUG
					hDLL = LoadLibraryExW(L"D3D11Device_d.dll", NULL, 0);
#else
					hDLL = LoadLibraryExW(L"D3D11Device.dll", NULL, 0);
#endif
					if (!hDLL)
					{
						LOG_ERROR("Could not find D3D11Device.dll!");
						MessageBox(NULL, "Loading 'D3D11Device.dll' failed.", "LongBowEngine - error", MB_OK | MB_ICONERROR);
						return RenderDevicePtr(nullptr);
					}
				}
					break;
				default:
				{
					LOG_ERROR("Renderer API is not supported!");
					MessageBox(NULL, "API is not supported.", "LongBowEngine - error", MB_OK | MB_ICONERROR);
					return RenderDevicePtr(nullptr);
				}
					break;
				}

				CREATERENDERDEVICE _CreateRenderDevice = 0;

				// Zeiger auf die dll Funktion 'CreateRenderDevice'
				_CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(hDLL, "CreateRenderDevice");

				GetWindowLong(GetActiveWindow(), 0);
				IRenderDevice* pDevice = _CreateRenderDevice(GetModuleHandle(NULL), EventLogger::GetInstance());

				// aufruf der dll Create-Funktionc
				if (pDevice == nullptr)
				{
					LOG_ERROR("Could not create Render Device from DLL!");
					return RenderDevicePtr(nullptr);
				}
				else
				{
					DeviceMap.insert(std::pair<API, RenderDevicePtr>(api, RenderDevicePtr(pDevice)));
					return DeviceMap[api];
				}
			}
			else
			{
				return DeviceMap[api];
			}
		}

	}
}