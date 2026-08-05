#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

//! Factory the ThinRenderDeviceManager resolves out of this plugin.
extern "C" __declspec(dllexport) IThinDevice *CreateThinDevice(EventLogger &logger, uint32_t deviceHandle)
{
    EventLogger::SetInstance(logger);

    D3D12ThinDevice *device = new D3D12ThinDevice();
    if (!device->Initialize(deviceHandle))
    {
        delete device;
        return nullptr;
    }

    return device;
}

} // namespace bow
