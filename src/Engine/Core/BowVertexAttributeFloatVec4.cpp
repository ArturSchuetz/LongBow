#pragma once
#include "BowVertexAttributeFloatVec4.h"

#include "BowMath.h"

namespace Bow {
	namespace Core {

		VertexAttributeFloatVec4::VertexAttributeFloatVec4(const std::string& name) : VertexAttribute<Vector4<float>>(name, VertexAttributeType::FloatVector4)
		{
		}

		VertexAttributeFloatVec4::VertexAttributeFloatVec4(const std::string& name, int capacity) : VertexAttribute<Vector4<float>>(name, VertexAttributeType::FloatVector4, capacity)
		{
		}

	}
}