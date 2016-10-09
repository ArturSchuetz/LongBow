#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

namespace Bow {
	namespace Scene{

		////////////////////////////////////////////////////
		// INTERFACES

		class ISceneManager;
		class IRenderStrategy;
			enum class RenderStrategyTyoe : char;
		typedef std::shared_ptr<IRenderStrategy> RenderStrategyPtr;
	}
}
