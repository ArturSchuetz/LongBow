#include "BowLogger.h"
#include "BowDIInputDevice.h"

namespace Bow {
	namespace Input {

		extern "C" __declspec(dllexport) IInputDevice* CreateInputDevice(Core::EventLogger& logger, Renderer::GraphicsWindowPtr window)
		{
			// set the new instance of the logger to prevent the creation of a new one inside this dll
			Core::EventLogger::SetInstance(logger);
			DIInputDevice* device = new DIInputDevice();

			if (!device->Init())
			{
				delete device;
				return nullptr;
			}
			return device;
		}

	}
}