#pragma once
#include "BowPrerequisites.h"
#include "BowNetworkPredeclares.h"

namespace Bow {
	namespace Network {

		class INetworkDevice
		{
		public:
			INetworkDevice(void){};
			virtual ~INetworkDevice(void){}
			virtual void VRelease(void) = 0;
		};
	}
}
