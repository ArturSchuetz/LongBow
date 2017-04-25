#include "BowWinSockNetworkDevice.h"
#include "BowLogger.h"

namespace Bow {
	namespace Network {

		using namespace Core;

		extern "C" __declspec(dllexport) INetworkDevice* CreateNetworkDevice(EventLogger& logger)
		{
			// set the new instance of the logger to prevent the creation of a new one inside this dll
			EventLogger::SetInstance(logger);
			WinSockNetworkDevice* device = new WinSockNetworkDevice();

			if (!device->Init())
			{
				delete device;
				return nullptr;
			}
			return device;
		}

	}
}