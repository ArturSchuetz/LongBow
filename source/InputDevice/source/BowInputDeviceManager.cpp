#include "InputDevice/BowInputDeviceManager.h"
#include "InputDevice/IBowInputDevice.h"

#include "CoreSystems/BowLogger.h"


#include <unordered_map>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow
{
static std::unordered_map<InputDeviceAPI, InputDevicePtr> DeviceMap;

// Function inside the DLL we want to call to create our Device-Object
extern "C"
{
    typedef IInputDevice *(*CREATEINPUTDEVICE)(EventLogger &logger);
}

InputDeviceManager::~InputDeviceManager()
{
    while (DeviceMap.begin() != DeviceMap.end())
    {
        ReleaseDevice(DeviceMap.begin()->first);
    }
    DeviceMap.clear();
}

void InputDeviceManager::ReleaseDevice(InputDeviceAPI api)
{
    FN("InputDeviceManager::ReleaseDevice");
    if (DeviceMap.find(api) != DeviceMap.end())
    {
        if (DeviceMap[api].get() != nullptr)
        {
            DeviceMap[api]->VRelease();
            DeviceMap.erase(api);
        }
    }
}

InputDeviceManager &InputDeviceManager::GetInstance()
{
    FN("InputDeviceManager::GetInstance");
    static InputDeviceManager instance;
    return instance;
}

InputDevicePtr InputDeviceManager::GetOrCreateDevice(InputDeviceAPI api)
{
    FN("InputDeviceManager::GetOrCreateDevice");
    if (DeviceMap.find(api) == DeviceMap.end())
    {
        HMODULE hDLL = NULL;
        switch (api)
        {
        case InputDeviceAPI::NativeInput:
        {
#ifdef _DEBUG
            hDLL = LoadLibraryExW(L"NativeInputDeviced.dll", NULL, 0);
#else
            hDLL = LoadLibraryExW(L"NativeInputDevice.dll", NULL, 0);
#endif
            if (!hDLL)
            {
#ifdef _DEBUG
                LOG_ERROR("Could not find NativeInputDevices.dll!");
#else
                LOG_ERROR("Could not find NativeInputDevice.dll!");
#endif
                DWORD errorCode = GetLastError();
                LOG_ERROR("LoadLibrary failed with error code: %s", std::to_string(errorCode).c_str());

                return InputDevicePtr(nullptr);
            }
        }
        break;

        case InputDeviceAPI::DirectInput:
        {
#ifdef _DEBUG
            hDLL = LoadLibraryExW(L"DirectInputDevices.dll", NULL, 0);
#else
            hDLL = LoadLibraryExW(L"DirectInputDevice.dll", NULL, 0);
#endif
            if (!hDLL)
            {
#ifdef _DEBUG
                LOG_ERROR("Could not find DirectInputDeviced.dll!");
#else
                LOG_ERROR("Could not find DirectInputDevice.dll!");
#endif
                DWORD errorCode = GetLastError();
                LOG_ERROR("LoadLibrary failed with error code: %s", std::to_string(errorCode).c_str());

                return InputDevicePtr(nullptr);
            }
        }
        break;

        default:
        {
            LOG_ERROR("Input API is not supported!");
            return InputDevicePtr(nullptr);
        }
        break;
        }

        CREATEINPUTDEVICE _CreateInputDevice = 0;

        // Zeiger auf die dll Funktion 'CreateRenderDevice'
        _CreateInputDevice = (CREATEINPUTDEVICE)GetProcAddress(hDLL, "CreateInputDevice");
        IInputDevice *pDevice = _CreateInputDevice(EventLogger::GetInstance());

        // aufruf der dll Create-Funktionc
        if (pDevice == nullptr)
        {
            LOG_ERROR("Could not create Input Device from DLL!");
            return InputDevicePtr(nullptr);
        }
        else
        {
            DeviceMap.insert(std::pair<InputDeviceAPI, InputDevicePtr>(api, InputDevicePtr(pDevice)));
            return DeviceMap[api];
        }
    }
    else
    {
        return DeviceMap[api];
    }
}

KeyboardPtr InputDeviceManager::CreateKeyboardObject(GraphicsWindowPtr window)
{
    FN("InputDeviceManager::CreateKeyboardObject");
    InputDevicePtr pDevice = GetOrCreateDevice(InputDeviceAPI::NativeInput);
    return pDevice->VCreateKeyboardObject(window);
}

MousePtr InputDeviceManager::CreateMouseObject(GraphicsWindowPtr window)
{
    FN("InputDeviceManager::CreateMouseObject");
    InputDevicePtr pDevice = GetOrCreateDevice(InputDeviceAPI::NativeInput);
    return pDevice->VCreateMouseObject(window);
}

} // namespace bow
