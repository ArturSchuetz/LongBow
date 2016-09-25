#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

namespace Bow {
	namespace Scene{

		/**
		* \~german
		**/
		class ISceneNode
		{
		public:
			ISceneNode(void){};
			virtual ~ISceneNode(void){}
			virtual void VRelease(void) = 0;
		};
	}
}
