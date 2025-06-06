/**
 * @file BowFaceCulling.h
 * @brief Declarations for BowFaceCulling.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "IBowRenderContext.h" // For WindingOrder

namespace bow {

	enum class CullFace : char
	{
		Front,
		Back,
		FrontAndBack
	};

	struct FaceCulling
	{
	public:
		FaceCulling()
		{
			Enabled = true;
			Face = CullFace::Back;
			FrontFaceWindingOrder = WindingOrder::Counterclockwise;
		}

		bool Enabled;
		CullFace Face;
		WindingOrder FrontFaceWindingOrder;
	};

}
