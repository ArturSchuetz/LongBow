#include "BowLogger.h"
#include "BowDirectX12RenderDevice.h"

namespace bow {

	extern "C" __declspec(dllexport) IRenderDevice* CreateRenderDevice(EventLogger& logger)
	{
		// set the new instance of the logger to prevent the creation of a new one inside this dll
		EventLogger::SetInstance(logger);
		DirectX12RenderDevice* device = new DirectX12RenderDevice();

		if (!device->Initialize())
		{
			delete device;
			return nullptr;
		}
		return device;
	}

}
