#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "BowStencilTestFace.h"

namespace Bow {
	namespace Renderer{

		struct StencilTest
		{
		public:
			StencilTest()
			{
				Enabled = false;
			}

			bool Enabled;
			StencilTestFace FrontFace;
			StencilTestFace BackFace;
		};

	}
}