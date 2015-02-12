#pragma once
#include "BowPrerequisites.h"

#include "IBowMouse.h"
#include "IBowKeyboard.h"

#include <dinput.h>

namespace Bow {
	namespace Input {

		enum class InputDeviceType : char
		{
			Mouse,
			Keyboard
		};

		class DIInputDeviceBase
		{
		public:
			DIInputDeviceBase(IDirectInput8 *directInput, HWND windowHandle);
			virtual ~DIInputDeviceBase(void) = 0;

		protected:
			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			void Release(void);
			bool Init(REFGUID rguid, LPCDIDATAFORMAT pDf);

			// Get Device Data
			bool GetData(InputDeviceType deviceType, void *pData, DWORD *NumElements);

			IDirectInputDevice8    *m_pDirectInputDevice;
			IDirectInput8		   *m_pDirectInput;
			HWND					m_windowHandle;
		};

	}
}