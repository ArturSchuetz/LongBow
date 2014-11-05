#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		enum class PixelBufferHint : char
		{
			Stream,
			Static,
			Dynamic
		};

	}
}