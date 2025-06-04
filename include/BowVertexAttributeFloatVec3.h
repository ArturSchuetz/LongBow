/**
 * @file BowVertexAttributeFloatVec3.h
 * @brief Declarations for BowVertexAttributeFloatVec3.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector3.h"

namespace bow {

	struct VertexAttributeFloatVec3 : VertexAttribute<Vector3<float>>
	{
	public:
		VertexAttributeFloatVec3(const std::string& name);
		VertexAttributeFloatVec3(const std::string& name, int capacity);
	};

}
