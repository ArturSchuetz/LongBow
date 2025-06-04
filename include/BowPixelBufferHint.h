/**
 * @file BowPixelBufferHint.h
 * @brief Declarations for BowPixelBufferHint.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	enum class PixelBufferHint : char
	{
		Stream,
		Static,
		Dynamic
	};

}
