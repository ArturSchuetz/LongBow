#pragma once
#include "BowPrerequisites.h"
#include "IBowInputDevice.h"

#include "IBowGraphicsWindow.h"
#include <dinput.h>

namespace Bow {
	namespace Input {

		class DIInputDevice : public IInputDevice
		{
		public:
			DIInputDevice();
			~DIInputDevice(void);

			bool Init();
			void VRelease(void) sealed;

			MousePtr	VCreateMouseObject(Renderer::GraphicsWindowPtr window);
			KeyboardPtr VCreateKeyboardObject(Renderer::GraphicsWindowPtr window);

		private:
			IDirectInput8 *m_pDirectInput;
		};

	}
}