#pragma once
#include "DirectInputDevice/DirectInputDevice_api.h"

#include <dinput.h>

namespace bow
{

enum class InputDeviceType : char
{
    Mouse,
    Keyboard
};

class DIInputDeviceBase
{
  public:
    DIInputDeviceBase(IDirectInput8 *directInput, HWND windowHandle);
    virtual ~DIInputDeviceBase() = 0;

  protected:
    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================
    void Release();
    bool Initialize(REFGUID rguid, LPCDIDATAFORMAT pDf);

    // Get Device Data
    bool GetData(InputDeviceType deviceType, void *pData, DWORD *NumElements);

    IDirectInputDevice8 *m_pDirectInputDevice;
    IDirectInput8 *m_pDirectInput;
    HWND m_windowHandle;

  private:
    // you shall not copy
    DIInputDeviceBase(const DIInputDeviceBase &obj) = delete;
    DIInputDeviceBase &operator=(const DIInputDeviceBase &obj) = delete;
};

} // namespace bow
