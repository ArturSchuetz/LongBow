#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer{

		enum class BufferHint : char
		{
			None,
			StreamDraw,
			StreamRead,
			StreamCopy,
			StaticDraw,
			StaticRead,
			StaticCopy,
			DynamicDraw,
			DynamicRead,
			DynamicCopy,
		};

	}
}