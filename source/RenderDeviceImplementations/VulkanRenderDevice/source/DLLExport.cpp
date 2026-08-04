#include <VulkanRenderDevice/BowVulkanRenderDevice.h>

#include <VulkanRenderDevice/VulkanLoaderLibrary.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

namespace bow
{
static LIBRARY_TYPE VulkanLibrary = nullptr;

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

    if (VulkanLibrary == nullptr)
    {
        if (!ConnectWithVulkanLoaderLibrary(VulkanLibrary))
        {
            return nullptr;
        }

        if (!LoadFunctionExportedFromVulkanLoaderLibrary(VulkanLibrary))
        {
            return nullptr;
        }

        if (!LoadGlobalLevelFunctions())
        {
            return nullptr;
        }
    }

    VulkanRenderDevice *device = new VulkanRenderDevice();
    if (!device->Initialize(deviceHandle))
    {
        delete device;
        return nullptr;
    }
    return device;
}

} // namespace bow
