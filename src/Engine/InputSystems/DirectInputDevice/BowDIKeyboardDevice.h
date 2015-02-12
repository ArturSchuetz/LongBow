#pragma once
#include "BowPrerequisites.h"
#include "BowDIInputDeviceBase.h"

#include <dinput.h>

namespace Bow {
	namespace Input {

		class DIKeyboardDevice : public DIInputDeviceBase, public IKeyboard
		{
		public:
			DIKeyboardDevice(IDirectInput8 *directInput, HWND windowHandle);
			~DIKeyboardDevice(void);

			bool Init(void);
			bool VUpdate(void);
			bool VIsPressed(Key keyID) const;

		protected:
			char m_keys[256];
		};

		typedef std::shared_ptr<DIKeyboardDevice> DIKeyboardPtr;
	}
}