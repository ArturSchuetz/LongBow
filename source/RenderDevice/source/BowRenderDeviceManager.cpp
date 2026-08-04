#include <RenderDevice/BowRenderDeviceManager.h>
#include <RenderDevice/IBowRenderDevice.h>

#include <CoreSystems/BowLogger.h>

#include <queue>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow
{
static std::vector<RenderDevicePtr> DeviceMap;

// Function inside the DLL we want to call to create our Device-Object
extern "C"
{
    typedef IRenderDevice *(*CREATERENDERDEVICE)(EventLogger &logger, uint32_t deviceHandle);
}

RenderDeviceManager::~RenderDeviceManager()
{
    while (DeviceMap.begin() != DeviceMap.end())
    {
        if (DeviceMap.begin()->get() != nullptr)
        {
            DeviceMap.begin()->get()->VRelease();
            DeviceMap.erase(DeviceMap.begin());
        }
    }
    DeviceMap.clear();
}

RenderDeviceManager &RenderDeviceManager::GetInstance()
{
    FN("RenderDeviceManager::GetInstance");
    static RenderDeviceManager instance;
    return instance;
}

namespace
{

//! Base filename of the plugin implementing each backend.
/*!
    The debug build appends a "d", matching the DEBUG_POSTFIX the CMake sets on
    every plugin target. Adding a backend means adding a line here and a case
    to RenderDeviceAPI, nothing else.
*/
const char *PluginName(RenderDeviceAPI api)
{
    switch (api)
    {
    case RenderDeviceAPI::DirectX11:
        return "DirectX11RenderDevice";
    case RenderDeviceAPI::DirectX12:
        return "DirectX12RenderDevice";
    case RenderDeviceAPI::OpenGL:
        return "OpenGLRenderDevice";
    case RenderDeviceAPI::Vulkan:
        return "VulkanRenderDevice";
    }
    return nullptr;
}

} // namespace

RenderDevicePtr RenderDeviceManager::CreateDevice(RenderDeviceAPI api, uint32_t deviceHandle)
{
    FN("RenderDeviceManager::CreateDevice");

    const char *pluginName = PluginName(api);
    if (pluginName == nullptr)
    {
        LOG_ERROR("Renderer API is not supported!");
        return RenderDevicePtr(nullptr);
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
        return RenderDevicePtr(nullptr);
    }

    CREATERENDERDEVICE _CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(hDLL, "CreateRenderDevice");
    if (_CreateRenderDevice == nullptr)
    {
        // Previously called through unconditionally, so a plugin that loaded
        // but exported nothing crashed instead of reporting itself.
        LOG_ERROR("%s does not export CreateRenderDevice.", libraryName.c_str());
        return RenderDevicePtr(nullptr);
    }

    IRenderDevice *pDevice = _CreateRenderDevice(EventLogger::GetInstance(), deviceHandle);
    if (pDevice == nullptr)
    {
        LOG_ERROR("Could not create Render Device from %s!", libraryName.c_str());
        return RenderDevicePtr(nullptr);
    }

    RenderDevicePtr devicePtr = RenderDevicePtr(pDevice);
    DeviceMap.push_back(devicePtr);
    return devicePtr;
}
} // namespace bow
