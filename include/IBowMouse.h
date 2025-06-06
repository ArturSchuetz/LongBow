/**
 * @file IBowMouse.h
 * @brief Declarations for IBowMouse.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowInputPredeclares.h"

#include "BowCorePredeclares.h"

namespace bow {
	
	enum class MouseButton : unsigned char
	{
		MOFS_BUTTON0,
		MOFS_BUTTON1,
		MOFS_BUTTON2,
		MOFS_BUTTON3,
		MOFS_BUTTON4,
		MOFS_BUTTON5,
		MOFS_BUTTON6,
		MOFS_BUTTON7
	};

	class IMouse
	{
	public:
		IMouse(void){};
		virtual ~IMouse(void){}

		virtual bool VUpdate(void) = 0;
		virtual bool VIsPressed(MouseButton keyID) const = 0;

		virtual Vector3<long> VGetRelativePosition() const = 0; // X and X are relative screen coordinates since last poll and Z represents the scroll wheel
		virtual Vector2<long> VGetAbsolutePosition() const = 0;
		virtual Vector2<long> VGetAbsolutePositionInsideWindow() const = 0;
		virtual bool VSetCursorPosition(int x, int y) = 0;

		virtual void VCageMouse(bool cage) = 0;
		virtual void VSetCage(long left, long top, long right, long bottom) = 0;
	};

}
