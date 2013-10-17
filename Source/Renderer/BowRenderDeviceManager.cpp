#include "LongBow.h"
#include "BowRenderDeviceManager.h"

namespace Bow {

	static std::hash_map<API, RenderDevicePtr> DeviceMap;

	RenderDeviceManager::RenderDeviceManager(void) { }

	RenderDeviceManager::~RenderDeviceManager(void)
	{
		FN("RenderDeviceManager::~RenderDeviceManager(void)");
		Release();
	}

	RenderDevicePtr RenderDeviceManager::GetOrCreateDevice(API api)
	{
		return GetOrCreateDevice(api, GetModuleHandle(0));
	}

	RenderDevicePtr RenderDeviceManager::GetOrCreateDevice(API api, HINSTANCE hInstance)
	{
		FN("RenderDeviceManager::GetOrCreateDevice(API api, HINSTANCE hInstance)");
		if(DeviceMap.find(api) == DeviceMap.end())
		{
			HMODULE hDLL = NULL;
			switch(api)
			{
			case API::OPENGL:
				{
					#ifdef _DEBUG
						hDLL = LoadLibraryExW(L"OGLRenderDevice_d.dll", NULL, 0);
					#else
						hDLL = LoadLibraryExW(L"OGLRenderDevice.dll", NULL, 0);
					#endif
					if(!hDLL)
					{
						LOG_ERROR("Could not find OGLRenderDevice.dll!");
						MessageBox(NULL, "Loading 'OGLRenderDevice.dll' failed.", "LongBowEngine - error", MB_OK | MB_ICONERROR);
						return RenderDevicePtr(nullptr);
					}
				}
				break;
			case API::DIRECT3D11:
				{
					#ifdef _DEBUG
						hDLL = LoadLibraryExW(L"D3D11Device_d.dll", NULL, 0);
					#else
						hDLL = LoadLibraryExW(L"D3D11Device.dll", NULL, 0);
					#endif
					if(!hDLL)
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
			_CreateRenderDevice = (CREATERENDERDEVICE) GetProcAddress(hDLL, "CreateRenderDevice");
			IRenderDevice* pDevice = nullptr;

			// aufruf der dll Create-Funktionc
			if(!_CreateRenderDevice(&pDevice, hInstance))
			{
				LOG_ERROR("Could not create Render Device from DLL!");
				MessageBox(NULL, "Create RenderDevice from DLL failed.", "LongBowEngine - error", MB_OK | MB_ICONERROR);
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

	void RenderDeviceManager::Release(void)
	{
		FN("RenderDeviceManager::Release(void)");
		while(DeviceMap.begin() != DeviceMap.end())
		{
			ReleaseDevice(DeviceMap.begin()->first);
		}
		DeviceMap.clear();
	}

	void RenderDeviceManager::ReleaseDevice(API api)
	{
		FN("RenderDeviceManager::ReleaseDevice(API api)");
		if(DeviceMap.find(api) != DeviceMap.end())
		{
			if(DeviceMap[api].get() != nullptr)
			{
				DeviceMap[api]->VRelease();
				DeviceMap.erase(api);
			}
		}
	}
}