#pragma once
#include "BowVertexAttributeFloat.h"

#include "BowMath.h"

namespace bow {
	
	VertexAttributeFloat::VertexAttributeFloat(const std::string& name) : VertexAttribute<float>(name, VertexAttributeType::Float)
	{
	}

	VertexAttributeFloat::VertexAttributeFloat(const std::string& name, int capacity) : VertexAttribute<float>(name, VertexAttributeType::Float, capacity)
	{
	}

}