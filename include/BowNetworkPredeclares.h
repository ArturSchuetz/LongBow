#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Network {

		class NetworkDeviceManager;

		class INetworkDevice;
		typedef std::shared_ptr<INetworkDevice> NetworkDevicePtr;
	}
}
