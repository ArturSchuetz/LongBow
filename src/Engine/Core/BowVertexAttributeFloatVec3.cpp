#pragma once
#include "BowVertexAttributeFloatVec3.h"

#include "BowMath.h"

namespace Bow {
	namespace Core {

		VertexAttributeFloatVec3::VertexAttributeFloatVec3(const std::string& name) : VertexAttribute<Vector3<float>>(name, VertexAttributeType::FloatVector3)
		{
		}

		VertexAttributeFloatVec3::VertexAttributeFloatVec3(const std::string& name, int capacity) : VertexAttribute<Vector3<float>>(name, VertexAttributeType::FloatVector3, capacity)
		{
		}

	}
}