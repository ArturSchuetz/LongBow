#include "BowRenderDeviceManager.h"
#include "BowLogger.h"

#include "IBowRenderDevice.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow
{

	static std::unordered_map<RenderDeviceAPI, RenderDevicePtr> DeviceMap;

	// Function inside the DLL we want to call to create our Device-Object
	extern "C"
	{
		typedef IRenderDevice* (*CREATERENDERDEVICE)(EventLogger& logger);
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

	RenderDevicePtr RenderDeviceManager::GetOrCreateDevice(RenderDeviceAPI api)
	{
		if (DeviceMap.find(api) == DeviceMap.end())
		{
			HMODULE hDLL = NULL;
			switch (api)
			{
			case RenderDeviceAPI::OpenGL3x:
				{
#ifdef _DEBUG
					hDLL = LoadLibraryExW(L"OGL3xRenderDevice_d.dll", NULL, 0);
#else
					hDLL = LoadLibraryExW(L"OGL3xRenderDevice.dll", NULL, 0);
#endif
					if (!hDLL)
					{
#ifdef _DEBUG
						LOG_ERROR("Could not find OGL3xRenderDevice_d.dll!");
#else
						LOG_ERROR("Could not find OGL3xRenderDevice.dll!");
#endif
						return RenderDevicePtr(nullptr);
					}
				}
				break;
			case RenderDeviceAPI::Vulkan:
				{
#ifdef _DEBUG
					hDLL = LoadLibraryExW(L"VulkanRenderDevice_d.dll", NULL, 0);
#else
					hDLL = LoadLibraryExW(L"VulkanRenderDevice.dll", NULL, 0);
#endif
					if (!hDLL)
					{
#ifdef _DEBUG
						LOG_ERROR("Could not find VulkanRenderDevice_d.dll!");
#else
						LOG_ERROR("Could not find VulkanRenderDevice.dll!");
#endif
						return RenderDevicePtr(nullptr);
					}
				}
				break;
			default:
				{
					LOG_ERROR("Renderer API is not supported!");
					return RenderDevicePtr(nullptr);
				}
				break;
			}

			CREATERENDERDEVICE _CreateRenderDevice = 0;

			// Zeiger auf die dll Funktion 'CreateRenderDevice'
			_CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(hDLL, "CreateRenderDevice");
			IRenderDevice *pDevice = _CreateRenderDevice(EventLogger::GetInstance());

			// aufruf der dll Create-Funktionc
			if (pDevice == nullptr)
			{
				LOG_ERROR("Could not create Render Device from DLL!");
				return RenderDevicePtr(nullptr);
			}
			else
			{
				DeviceMap.insert(std::pair<RenderDeviceAPI, RenderDevicePtr>(api, RenderDevicePtr(pDevice)));
				return DeviceMap[api];
			}
		}
		else
		{
			return DeviceMap[api];
		}
	}

	void RenderDeviceManager::ReleaseDevice(RenderDeviceAPI api)
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
}
