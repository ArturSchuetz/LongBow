#pragma once
#include "BowPrerequisites.h"

#include "BowBlending.h"
#include "BowColorMask.h"
#include "BowDepthRange.h"
#include "BowDepthTest.h"
#include "BowFaceCulling.h"
#include "BowPrimitiveRestart.h"
#include "BowScissorTest.h"
#include "BowStencilTest.h"
#include "BowStencilTestFace.h"

namespace Bow {
	namespace Renderer{

		enum class ProgramPointSize : char
		{
			Enabled,
			Disabled
		};

		enum class RasterizationMode : char
		{
			Point,
			Line,
			Fill
		};

		struct RenderState
		{
		public:
			RenderState() : ColorMask(true, true, true, true)
			{
				ProgramPointSize = ProgramPointSize::Disabled;
				RasterizationMode = RasterizationMode::Fill;
				DepthMask = true;
			}

			PrimitiveRestart PrimitiveRestart;
			FaceCulling FaceCulling;
			ProgramPointSize ProgramPointSize;
			RasterizationMode RasterizationMode;
			ScissorTest ScissorTest;
			StencilTest StencilTest;
			DepthTest DepthTest;
			DepthRange DepthRange;
			Blending Blending;
			ColorMask ColorMask;
			bool DepthMask;
		};

	}
}