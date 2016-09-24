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
		class ISceneManager
		{
		public:
			ISceneManager(void){};
			virtual ~ISceneManager(void){}
			virtual void VRelease(void) = 0;

			virtual void VCreateMesh(void) = 0;
			virtual void VDrawAll(void) = 0;
		};
	}
}