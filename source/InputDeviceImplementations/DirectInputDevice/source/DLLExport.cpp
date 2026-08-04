#include "DirectInputDevice/BowDIInputDevice.h"

#include "CoreSystems/BowLogger.h"


namespace bow
{

extern "C" __declspec(dllexport) IInputDevice *CreateInputDevice(EventLogger &logger)
{
    FN("CreateInputDevice");

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

    DIInputDevice *device = new DIInputDevice();
    if (!device->Initialize())
    {
        delete device;
        return nullptr;
    }
    return device;
}

} // namespace bow
