/**
 * @file BowVertexAttributeFloatVec2.h
 * @brief Declarations for BowVertexAttributeFloatVec2.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector2.h"

namespace bow {

	struct VertexAttributeFloatVec2 : VertexAttribute<Vector2<float>>
	{
	public:
		VertexAttributeFloatVec2(const std::string& name);
		VertexAttributeFloatVec2(const std::string& name, int capacity);
	};

}
