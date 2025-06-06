/**
 * @file BowNetworkPredeclares.h
 * @brief Declarations for BowNetworkPredeclares.
 */

#pragma once
#include "BowPrerequisites.h"

namespace bow {

	class NetworkDeviceManager;

	class INetworkDevice;
	typedef std::shared_ptr<INetworkDevice> NetworkDevicePtr;
}
