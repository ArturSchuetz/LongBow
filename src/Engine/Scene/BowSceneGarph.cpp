#include "BowSceneGraph.h"
#include "BowLogger.h"

// Render Strategies
#include "BowLegacyVertexLitRenderer.h"

namespace Bow {
	namespace Scene {

		void error_callback(int error, const char* description)
		{
			LOG_ERROR(description);
		}

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

		void SceneGraph::SetRenderStrategy(RenderStrategyType strategy)
		{
			switch (strategy)
			{
			case RenderStrategyType::LagicyVertexLit:
				m_pRenderStrategy = LegacyVertexLitRendererPtr(new LegacyVertexLitRenderer());
				break;
			default:
				m_pRenderStrategy = LegacyVertexLitRendererPtr(new LegacyVertexLitRenderer());
				break;
			}
		}
	}
}
