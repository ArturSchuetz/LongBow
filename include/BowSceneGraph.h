#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

#include "IBowRenderStrategy.h"

namespace Bow {
	namespace Scene{

		/**
		* \~german
		**/
		class SceneGraph
		{
		public:
			SceneGraph(void);
			virtual ~SceneGraph(void);

			virtual bool VInit(void);
			virtual void VRelease(void);

			void SetRenderStrategy(RenderStrategyType strategy);

		private:
			RenderStrategyPtr	m_pRenderStrategy;
		};
	}
}