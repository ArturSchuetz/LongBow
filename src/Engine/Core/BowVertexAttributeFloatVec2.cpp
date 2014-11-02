#pragma once
#include "BowVertexAttributeFloatVec2.h"

#include "BowMath.h"

namespace Bow {
	namespace Core {

		VertexAttributeFloatVec2::VertexAttributeFloatVec2(const std::string& name) : VertexAttribute<Vector2<float>>(name, VertexAttributeType::FloatVector2)
		{
		}

		VertexAttributeFloatVec2::VertexAttributeFloatVec2(const std::string& name, int capacity) : VertexAttribute<Vector2<float>>(name, VertexAttributeType::FloatVector2, capacity)
		{
		}

	}
}