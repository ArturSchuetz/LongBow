#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector3.h"

namespace Bow {
	namespace Core {

		struct VertexAttributeFloatVec3 : VertexAttribute<Vector3<float>>
		{
		public:
			VertexAttributeFloatVec3(const std::string& name);
			VertexAttributeFloatVec3(const std::string& name, int capacity);
		};

	}
}