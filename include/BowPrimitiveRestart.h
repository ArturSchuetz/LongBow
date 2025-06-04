/**
 * @file BowPrimitiveRestart.h
 * @brief Declarations for BowPrimitiveRestart.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	struct PrimitiveRestart
	{
	public:
		PrimitiveRestart()
		{
			Enabled = false;
			Index = 0;
		}

		bool Enabled;
		int Index;
	};

}
