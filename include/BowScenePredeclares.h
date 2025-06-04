/**
 * @file BowScenePredeclares.h
 * @brief Declarations for BowScenePredeclares.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

namespace bow {
	namespace Scene{

		////////////////////////////////////////////////////
		// INTERFACES

		class ISceneManager;
		class IRenderStrategy;
			enum class RenderStrategyTyoe : char;
		typedef std::shared_ptr<IRenderStrategy> RenderStrategyPtr;
	}
}
