#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

//! Factory the ThinRenderDeviceManager resolves out of this plugin.
/*!
    Named CreateThinDevice rather than CreateRenderDevice so that a thin
    backend cannot be loaded by the classic manager, or the other way round.
    The logger is handed in because a singleton does not cross a
    shared-library boundary on Windows.
*/
extern "C" __declspec(dllexport) IThinDevice *CreateThinDevice(EventLogger &logger, uint32_t deviceHandle)
{
    EventLogger::SetInstance(logger);

    VulkanThinDevice *device = new VulkanThinDevice();
    if (!device->Initialize(deviceHandle))
    {
        delete device;
        return nullptr;
    }

    return device;
}

} // namespace bow
