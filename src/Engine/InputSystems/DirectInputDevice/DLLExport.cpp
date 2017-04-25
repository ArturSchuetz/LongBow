#include "BowDIInputDevice.h"
#include "BowLogger.h"

namespace Bow {
	namespace Input {

		using namespace Core;

		extern "C" __declspec(dllexport) IInputDevice* CreateInputDevice(Core::EventLogger& logger)
		{
			// set the new instance of the logger to prevent the creation of a new one inside this dll
			EventLogger::SetInstance(logger);
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