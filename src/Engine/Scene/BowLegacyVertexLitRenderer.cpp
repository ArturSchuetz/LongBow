#include "BowLegacyVertexLitRenderer.h"
#include "BowLogger.h"

namespace Bow {
	namespace Scene {

		void error_callback(int error, const char* description)
		{
			LOG_ERROR(description);
		}

		LegacyVertexLitRenderer::LegacyVertexLitRenderer(void)
		{
			
		}

		LegacyVertexLitRenderer::~LegacyVertexLitRenderer(void)
		{

		}
	}
}