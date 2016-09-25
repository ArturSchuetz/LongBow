#include "BowLegacyVertexLitRenderer.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

namespace Bow {
	namespace Scene {

		using namespace Core;

		void error_callback(int error, const char* description)
		{
			LOG_ERROR(description);
		}

		BowLegacyVertexLitRenderer::BowLegacyVertexLitRenderer(void)
		{
			
		}

		BowLegacyVertexLitRenderer::~BowLegacyVertexLitRenderer(void)
		{
		}
	}
}