/**
 * @file BowSceneGraph.h
 * @brief Declarations for BowSceneGraph.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

namespace bow {
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

		private:
			RenderStrategyPtr	m_pRenderStrategy;
		};
	}
}