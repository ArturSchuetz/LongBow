/**
 * @file BowVertexAttributeFloat.h
 * @brief Declarations for BowVertexAttributeFloat.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector2.h"

namespace bow {

	struct VertexAttributeFloat : VertexAttribute<float>
	{
	public:
		VertexAttributeFloat(const std::string& name);
		VertexAttributeFloat(const std::string& name, int capacity);
	};

}
