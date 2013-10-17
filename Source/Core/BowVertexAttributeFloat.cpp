#pragma once
#include "BowVertexAttributeFloat.h"

namespace Bow {

	VertexAttributeFloat::VertexAttributeFloat(std::string name) : VertexAttribute<float>(name, VertexAttributeType::Float)
    {
    }

    VertexAttributeFloat::VertexAttributeFloat(std::string name, int capacity) : VertexAttribute<float>(name, VertexAttributeType::Float, capacity)
    {
    }

}