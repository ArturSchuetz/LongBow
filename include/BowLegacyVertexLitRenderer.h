#pragma once
#include "BowPrerequisites.h"

#include "IBowRenderStrategy.h"

namespace Bow {
	namespace Scene {

		class BowLegacyVertexLitRenderer : IRenderStrategy
		{
		public:
			BowLegacyVertexLitRenderer(void);
			~BowLegacyVertexLitRenderer(void);

		private:		
			//you shall not copy
			BowLegacyVertexLitRenderer(BowLegacyVertexLitRenderer&){}
			BowLegacyVertexLitRenderer& operator=(const BowLegacyVertexLitRenderer&) { return *this; }
		};
	}
}