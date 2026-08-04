#include <DirectInputDevice/BowDIInputDeviceBase.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

DIInputDeviceBase::DIInputDeviceBase(IDirectInput8 *directInput, HWND windowHandle)
{
    FN("DIInputDeviceBase::DIInputDeviceBase");

    m_pDirectInput = directInput;
    m_windowHandle = windowHandle;
    m_pDirectInputDevice = nullptr;
}

DIInputDeviceBase::~DIInputDeviceBase()
{
    FN("DIInputDeviceBase::~DIInputDeviceBase");

    Release();
}

bool DIInputDeviceBase::Initialize(REFGUID rguid, LPCDIDATAFORMAT pDf)
{
    FN("DIInputDeviceBase::Initialize");

    // Get cooperative access if window is in foreground
    DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

    if (m_pDirectInputDevice)
    {
        m_pDirectInputDevice->Unacquire();
        m_pDirectInputDevice->Release();
        m_pDirectInputDevice = nullptr;
    }

    // Create Device
    if (FAILED(m_pDirectInput->CreateDevice(rguid, &m_pDirectInputDevice, NULL)))
    {
        return false;
    }

    if (FAILED(m_pDirectInputDevice->SetDataFormat(pDf)))
    {
        return false;
    }

    if (FAILED(m_pDirectInputDevice->SetCooperativeLevel(m_windowHandle, dwFlags)))
    {
        return false;
    }

    return true;
}

void DIInputDeviceBase::Release()
{
    FN("DIInputDeviceBase::Release");

    if (m_pDirectInputDevice)
    {
        m_pDirectInputDevice->Unacquire();
        m_pDirectInputDevice->Release();
        m_pDirectInputDevice = nullptr;
    }
}

bool DIInputDeviceBase::GetData(InputDeviceType deviceType, void *pData, DWORD *NumElements)
{
    FN("DIInputDeviceBase::GetData");

    HRESULT result;
    size_t size = 0;

    if (deviceType == InputDeviceType::Mouse)
    {
        size = sizeof(DIDEVICEOBJECTDATA);

        result = m_pDirectInputDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA *)pData, NumElements, 0);
    }
    else
    {
        if (deviceType == InputDeviceType::Keyboard)
        {
            size = sizeof(char) * 256;
        }
        else
        {
            return false;
        }

        result = m_pDirectInputDevice->GetDeviceState(size, pData);
    }

    if (FAILED(result))
    {
        if ((result == DIERR_NOTACQUIRED) || (result == DIERR_INPUTLOST))
        {
            result = m_pDirectInputDevice->Acquire();
            while (result == DIERR_INPUTLOST)
            {
                result = m_pDirectInputDevice->Acquire();
            }

            // Another application occupies this Device
            if (result == DIERR_OTHERAPPHASPRIO)
            {
                return true;
            }

            // yeah, we got it back!
            if (SUCCEEDED(result))
            {
                if (deviceType == InputDeviceType::Mouse)
                {
                    result = m_pDirectInputDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA *)pData, NumElements, 0);
                }
                else
                {
                    result = m_pDirectInputDevice->GetDeviceState(size, pData);
                }
            }

            if (FAILED(result))
            {
                return false;
            }
        }
        else
        {
            // another error occured
            return false;
        }
    }

    return true;
}

} // namespace bow
