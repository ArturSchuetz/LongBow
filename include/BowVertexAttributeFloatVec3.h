#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"

namespace Bow {
	namespace Core {

		struct VertexAttributeFloatVec3 : VertexAttribute<FloatVector3>
		{
		public:
			VertexAttributeFloatVec3(std::string name);
			VertexAttributeFloatVec3(std::string name, int capacity);
		};

	}
}