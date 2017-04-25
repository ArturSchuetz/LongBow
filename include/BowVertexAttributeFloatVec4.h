#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector3.h"

namespace Bow {
	namespace Core {

		struct VertexAttributeFloatVec4 : VertexAttribute<Vector4<float>>
		{
		public:
			VertexAttributeFloatVec4(const std::string& name);
			VertexAttributeFloatVec4(const std::string& name, int capacity);
		};

	}
}