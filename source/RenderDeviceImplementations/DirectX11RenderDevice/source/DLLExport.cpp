#include <DirectX11RenderDevice/BowD3D11RenderDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

//! Factory the RenderDeviceManager resolves out of this plugin.
/*!
    The logger instance is handed in because a singleton does not cross a
    shared-library boundary on Windows: without this the plugin would build its
    own logger and write to its own file.
*/
extern "C" __declspec(dllexport) IRenderDevice *CreateRenderDevice(EventLogger &logger, uint32_t deviceHandle)
{
    EventLogger::SetInstance(logger);

    D3D11RenderDevice *device = new D3D11RenderDevice();
    if (!device->Initialize(deviceHandle))
    {
        delete device;
        return nullptr;
    }

    return device;
}

} // namespace bow
