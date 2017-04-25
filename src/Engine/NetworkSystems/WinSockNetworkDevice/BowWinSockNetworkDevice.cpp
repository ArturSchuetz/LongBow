#include "BowWinSockNetworkDevice.h"
#include "BowLogger.h"

namespace Bow {
	namespace Network {

		WinSockNetworkDevice::WinSockNetworkDevice(void)
		{

		}

		WinSockNetworkDevice::~WinSockNetworkDevice(void)
		{
			VRelease();
		}

		void WinSockNetworkDevice::VRelease(void)
		{
			LOG_TRACE("WinSockNetworkDevice released.");
		}

		bool WinSockNetworkDevice::Init(void)
		{
			LOG_TRACE("WinSock sucessfully initialized!");
			return true;
		}
	}
}