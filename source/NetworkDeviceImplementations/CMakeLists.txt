# Network backends, opened by name at runtime by NetworkDeviceManager.

if(WIN32)
    add_subdirectory(WinSockNetworkDevice)
endif()
