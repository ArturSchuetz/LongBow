#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

namespace Bow {
	namespace Scene{

		enum class RenderStrategyType : char
		{
			LagicyVertexLit
		};

		/**
		* \~german
		**/
		class IRenderStrategy
		{
		public:
			IRenderStrategy(void){};
			virtual ~IRenderStrategy(void){}
		};
	}
}