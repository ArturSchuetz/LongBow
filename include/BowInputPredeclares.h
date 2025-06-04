/**
 * @file BowInputPredeclares.h
 * @brief Declarations for BowInputPredeclares.
 */

#pragma once
#include "BowPrerequisites.h"

namespace bow {

	class InputDeviceManager;
		enum class InputDeviceAPI : char;

	class IInputDevice;
	typedef std::shared_ptr<IInputDevice> InputDevicePtr;

	class IKeyboard;
	typedef std::shared_ptr<IKeyboard> KeyboardPtr;

	class IMouse;
	typedef std::shared_ptr<IMouse> MousePtr;
}
