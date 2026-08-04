#include <OpenGL3xRenderDevice/BowOGL3xRenderDevice.h>

#include <CoreSystems/BowLogger.h>


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
            LOG_WARNING("Logger instance already exists, but it is different "
                        "from the one passed as argument");
        }
    }

    OGLRenderDevice *device = new OGLRenderDevice();
    if (!device->Initialize())
    {
        delete device;
        return nullptr;
    }
    return device;
}

} // namespace bow
