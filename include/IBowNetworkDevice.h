/**
 * @file IBowNetworkDevice.h
 * @brief Declarations for IBowNetworkDevice.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowNetworkPredeclares.h"

namespace bow {

	class INetworkDevice
	{
	public:
		INetworkDevice(void){};
		virtual ~INetworkDevice(void){}
		virtual void VRelease(void) = 0;
	};
}
