#include <DirectX12RenderDevice/BowDirectX12RenderDevice.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

namespace bow
{

extern "C" __declspec(dllexport) IRenderDevice *CreateRenderDevice(EventLogger &logger, uint32_t deviceHandle)
{
    FN("CreateRenderDevice");

    // set the new instance of the logger to prevent the creation of a new one
    // inside this dll
    if (!EventLogger::HasInstance())
    {
        EventLogger::SetInstance(logger);
    }
    else
    {
        if (&(EventLogger::GetInstance()) != &logger)
        {
            LOG_WARNING("Logger instance already exists, but it is different from the one passed as argument");
        }
    }

    DirectX12RenderDevice *device = new DirectX12RenderDevice();
    if (!device->Initialize(deviceHandle))
    {
        delete device;
        return nullptr;
    }
    return device;
}

} // namespace bow
