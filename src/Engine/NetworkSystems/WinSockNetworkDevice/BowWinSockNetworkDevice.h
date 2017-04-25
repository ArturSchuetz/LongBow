#pragma once
#include "BowPrerequisites.h"

#include "IBowNetworkDevice.h"

namespace Bow {
	namespace Network {

		class WinSockNetworkDevice : public INetworkDevice
		{
		public:
			WinSockNetworkDevice(void);
			~WinSockNetworkDevice(void);

			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			bool Init(void);
			void VRelease(void);

		private:
			//you shall not copy
			WinSockNetworkDevice(WinSockNetworkDevice&) {}
			WinSockNetworkDevice& operator=(const WinSockNetworkDevice&) { return *this; }
		};

		typedef std::shared_ptr<WinSockNetworkDevice> WinSockNetworkDevicePtr;
		typedef std::unordered_map<unsigned int, WinSockNetworkDevice> WinSockNetworkDeviceMap;

	}
}