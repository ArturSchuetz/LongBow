#include "BowSceneGraph.h"
#include "BowLogger.h"


namespace Bow {
	namespace Scene {

		SceneGraph::SceneGraph(void)
		{
			
		}

		SceneGraph::~SceneGraph(void)
		{
			VRelease();
		}

		bool SceneGraph::VInit(void)
		{
			return true;
		}

		void SceneGraph::VRelease(void)
		{

		}
	}
}
