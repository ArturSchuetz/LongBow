#pragma once
#include "BowPrerequisites.h"
#include "BowInputPredeclares.h"

#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Input {

		//! \brief InputDevice creates the real objects inside of the dll.
		//! maybe I have to change this interface to support exotic input 
		//! devices with a dll for one specific device like the Oculus Rift,
		//! but at this moment I don't support any.
		class IInputDevice
		{
		public:
			IInputDevice(){}
			virtual ~IInputDevice(void){}
			virtual void VRelease() = 0;

			virtual MousePtr VCreateMouseObject(Renderer::GraphicsWindowPtr window) = 0;
			virtual KeyboardPtr VCreateKeyboardObject(Renderer::GraphicsWindowPtr window) = 0;
		};
	}
}