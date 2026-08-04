#include <RenderDevice/BowRenderDeviceManager.h>
#include <RenderDevice/IBowRenderDevice.h>

#include <CoreSystems/BowLogger.h>

#include <queue>

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

RenderDevicePtr RenderDeviceManager::CreateDevice(RenderDeviceAPI api, uint32_t deviceHandle)
{
    FN("RenderDeviceManager::CreateDevice");
    HMODULE hDLL = NULL;
    switch (api)
    {
    case RenderDeviceAPI::DirectX12:
    {
#ifdef _DEBUG
        hDLL = LoadLibrary("DirectX12RenderDeviced.dll");
#else
        hDLL = LoadLibrary("DirectX12RenderDevice.dll");
#endif
        if (!hDLL)
        {
#ifdef _DEBUG
            LOG_ERROR("Could not find DirectX12RenderDeviced.dll!");
#else
            LOG_ERROR("Could not find DirectX12RenderDevice.dll!");
#endif
            DWORD errorCode = GetLastError();
            LOG_ERROR("LoadLibrary failed with error code: %s", std::to_string(errorCode).c_str());

            return RenderDevicePtr(nullptr);
        }
    }
    break;
    case RenderDeviceAPI::OpenGL3x:
    {
#ifdef _DEBUG
        hDLL = LoadLibrary("OpenGL3xRenderDeviced.dll");
#else
        hDLL = LoadLibrary("OpenGL3xRenderDevice.dll");
#endif
        if (!hDLL)
        {
#ifdef _DEBUG
            LOG_ERROR("Could not find OpenGL3xRenderDeviced.dll!");
#else
            LOG_ERROR("Could not find OpenGL3xRenderDevice.dll!");
#endif
            DWORD errorCode = GetLastError();
            LOG_ERROR("LoadLibrary failed with error code: %s", std::to_string(errorCode).c_str());

            return RenderDevicePtr(nullptr);
        }
    }
    break;
    case RenderDeviceAPI::Vulkan:
    {
#ifdef _DEBUG
        hDLL = LoadLibrary("VulkanRenderDeviced.dll");
#else
        hDLL = LoadLibrary("VulkanRenderDevice.dll");
#endif
        if (!hDLL)
        {
#ifdef _DEBUG
            LOG_ERROR("Could not find VulkanRenderDeviced.dll!");
#else
            LOG_ERROR("Could not find VulkanRenderDevice.dll!");
#endif
            DWORD errorCode = GetLastError();
            LOG_ERROR("LoadLibrary failed with error code: %s", std::to_string(errorCode).c_str());

            return RenderDevicePtr(nullptr);
        }
    }
    break;
    default:
    {
        LOG_ERROR("Renderer API is not supported!");
        return RenderDevicePtr(nullptr);
    }
    break;
    }

    CREATERENDERDEVICE _CreateRenderDevice = 0;

    // Zeiger auf die dll Funktion 'CreateRenderDevice'
    _CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(hDLL, "CreateRenderDevice");
    IRenderDevice *pDevice = _CreateRenderDevice(EventLogger::GetInstance(), deviceHandle);

    // aufruf der dll Create-Funktionc
    if (pDevice == nullptr)
    {
        LOG_ERROR("Could not create Render Device from DLL!");
        return RenderDevicePtr(nullptr);
    }

    RenderDevicePtr devicePtr = RenderDevicePtr(pDevice);
    DeviceMap.push_back(devicePtr);
    return devicePtr;
}
} // namespace bow
