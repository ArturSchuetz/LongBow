/**
 * @file BowDepthRange.h
 * @brief Declarations for BowDepthRange.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	struct DepthRange
	{
	public:

		DepthRange()
		{
			Near = 0.0;
			Far = 1.0;
		}

		double Near;
		double Far;
	};

}
