#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Input {
		class IInputDevice;
		typedef std::shared_ptr<IInputDevice> InputDevicePtr;

		class IKeyboard;
		typedef std::shared_ptr<IKeyboard> KeyboardPtr;

		class IMouse;
		typedef std::shared_ptr<IMouse> MousePtr;
	}
}