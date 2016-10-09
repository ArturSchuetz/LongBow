#pragma once
#include "BowPrerequisites.h"
#include "BowScenePredeclares.h"

#include "IBowRenderStrategy.h"

namespace Bow {
	namespace Scene {

		class LegacyVertexLitRenderer : public IRenderStrategy
		{
		public:
			LegacyVertexLitRenderer(void);
			~LegacyVertexLitRenderer(void);

		private:		
			//you shall not copy
			LegacyVertexLitRenderer(LegacyVertexLitRenderer&){}
			LegacyVertexLitRenderer& operator=(const LegacyVertexLitRenderer&) { return *this; }
		};

		typedef std::shared_ptr<LegacyVertexLitRenderer> LegacyVertexLitRendererPtr;
	}
}