#include "BowWinSockNetworkDevice.h"
#include "BowLogger.h"

namespace bow {

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

	bool WinSockNetworkDevice::Initialize(void)
	{
		LOG_TRACE("WinSock sucessfully initialized!");
		return true;
	}
}
