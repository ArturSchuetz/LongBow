#include "BowSceneManager.h"
#include "BowLogger.h"

// Render Strategies
#include "BowLegacyVertexLitRenderer.h"

namespace Bow {
	namespace Scene {

		void error_callback(int error, const char* description)
		{
			LOG_ERROR(description);
		}

		SceneManager::SceneManager(void)
		{
			
		}

		SceneManager::~SceneManager(void)
		{
			VRelease();
		}

		bool SceneManager::VInit(void)
		{
			return true;
		}

		void SceneManager::VRelease(void)
		{

		}

		void SceneManager::SetRenderStrategy(RenderStrategyType strategy)
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
