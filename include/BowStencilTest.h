/**
 * @file BowStencilTest.h
 * @brief Declarations for BowStencilTest.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "BowStencilTestFace.h"

namespace bow {

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
