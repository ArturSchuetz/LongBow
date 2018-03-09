#pragma once
#include "BowPrerequisites.h"

#include "BowDIInputDeviceBase.h"

namespace bow {

	class DITypeConverter
	{
	public:
		static unsigned int To(Key key);
		static unsigned int To(MouseButton key);
	};

}
