#pragma once
#include "BowVertexAttributeFloatVec3.h"

#include "BowMath.h"

namespace Bow {
	namespace Core {

		VertexAttributeFloatVec3::VertexAttributeFloatVec3(std::string name) : VertexAttribute<FloatVector3>(name, VertexAttributeType::Float)
		{
		}

		VertexAttributeFloatVec3::VertexAttributeFloatVec3(std::string name, int capacity) : VertexAttribute<FloatVector3>(name, VertexAttributeType::Float, capacity)
		{
		}

	}
}