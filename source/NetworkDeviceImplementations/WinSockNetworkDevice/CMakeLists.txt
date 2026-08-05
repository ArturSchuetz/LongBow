# Networking over WinSock. Windows only.

longbow_add_plugin(WinSockNetworkDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::NetworkDevice
        ws2_32
    FOLDER "Backends/Network"
)
