#pragma once
#include "BowVertexAttributeFloatVec3.h"

namespace Bow {
	namespace Core {

		VertexAttributeFloatVec3::VertexAttributeFloatVec3(std::string name) : VertexAttribute<float[3]>(name, VertexAttributeType::Float)
		{
		}

		VertexAttributeFloatVec3::VertexAttributeFloatVec3(std::string name, int capacity) : VertexAttribute<float[3]>(name, VertexAttributeType::Float, capacity)
		{
		}

	}
}