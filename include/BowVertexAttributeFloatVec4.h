#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector3.h"

namespace bow {

	struct VertexAttributeFloatVec4 : VertexAttribute<Vector4<float>>
	{
	public:
		VertexAttributeFloatVec4(const std::string& name);
		VertexAttributeFloatVec4(const std::string& name, int capacity);
	};

}
