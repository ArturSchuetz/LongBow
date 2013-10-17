#pragma once
#include "LongBow.h"
#include "IBowRenderDevice.h"

namespace Bow {

	enum class API : char
	{
		DIRECT3D9 = 0,
		DIRECT3D11,
		OPENGL
	};

	class RenderDeviceManager
	{
	public:
		RenderDeviceManager(void);
		~RenderDeviceManager(void);
		void Release(void);

		RenderDevicePtr GetOrCreateDevice(API api);
		RenderDevicePtr GetOrCreateDevice(API api, HINSTANCE hInstance);
		void ReleaseDevice(API api);
	};

	typedef std::shared_ptr<RenderDeviceManager> RenderDeviceManagerPtr;

	extern "C"
	{
		typedef bool (*CREATERENDERDEVICE)(IRenderDevice **pInterface, HINSTANCE hInstance);
	}

}