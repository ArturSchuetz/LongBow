#include <ThinRenderDevice/BowThinRenderDeviceManager.h>

#include <ThinRenderDevice/IBowThinDevice.h>

#include <CoreSystems/BowLogger.h>

#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow
{

static std::vector<ThinDevicePtr> DeviceMap;

extern "C"
{
    typedef IThinDevice *(*CREATETHINDEVICE)(EventLogger &logger, uint32_t deviceHandle);
}

namespace
{

//! Base filename of the plugin implementing each thin backend.
const char *PluginName(ThinRenderDeviceAPI api)
{
    switch (api)
    {
    case ThinRenderDeviceAPI::DirectX12:
        return "DirectX12ThinDevice";
    case ThinRenderDeviceAPI::Vulkan:
        return "VulkanThinDevice";
    }
    return nullptr;
}

} // namespace

ThinRenderDeviceManager::~ThinRenderDeviceManager()
{
    while (!DeviceMap.empty())
    {
        if (DeviceMap.back() != nullptr)
        {
            DeviceMap.back()->VRelease();
        }
        DeviceMap.pop_back();
    }
}

ThinRenderDeviceManager &ThinRenderDeviceManager::GetInstance()
{
    FN("ThinRenderDeviceManager::GetInstance");

    static ThinRenderDeviceManager instance;
    return instance;
}

ThinDevicePtr ThinRenderDeviceManager::CreateDevice(ThinRenderDeviceAPI api, uint32_t deviceHandle)
{
    FN("ThinRenderDeviceManager::CreateDevice");

    const char *pluginName = PluginName(api);
    if (pluginName == nullptr)
    {
        LOG_ERROR("Thin renderer API is not supported!");
        return nullptr;
    }

#ifdef _DEBUG
    const std::string libraryName = std::string(pluginName) + "d.dll";
#else
    const std::string libraryName = std::string(pluginName) + ".dll";
#endif

    HMODULE hDLL = LoadLibrary(libraryName.c_str());
    if (!hDLL)
    {
        LOG_ERROR("Could not load %s (error code %s).", libraryName.c_str(), std::to_string(GetLastError()).c_str());
        return nullptr;
    }

    CREATETHINDEVICE createThinDevice = (CREATETHINDEVICE)GetProcAddress(hDLL, "CreateThinDevice");
    if (createThinDevice == nullptr)
    {
        LOG_ERROR("%s does not export CreateThinDevice.", libraryName.c_str());
        return nullptr;
    }

    IThinDevice *device = createThinDevice(EventLogger::GetInstance(), deviceHandle);
    if (device == nullptr)
    {
        LOG_ERROR("Could not create a thin device from %s!", libraryName.c_str());
        return nullptr;
    }

    ThinDevicePtr devicePtr = ThinDevicePtr(device);
    DeviceMap.push_back(devicePtr);
    return devicePtr;
}

} // namespace bow
