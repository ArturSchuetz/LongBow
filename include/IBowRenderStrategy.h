#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

#include "BowRendererPredeclares.h"
#include "BowCorePredeclares.h"

namespace Bow {
	namespace Scene{

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