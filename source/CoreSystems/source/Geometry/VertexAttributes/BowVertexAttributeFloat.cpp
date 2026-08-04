#pragma once
#include "CoreSystems/Geometry/VertexAttributes/BowVertexAttributeFloat.h"



namespace bow
{

VertexAttributeFloat::VertexAttributeFloat(const std::string &name) : VertexAttribute<float>(name, VertexAttributeType::Float) { FN("VertexAttributeFloat::VertexAttributeFloat"); }

VertexAttributeFloat::VertexAttributeFloat(const std::string &name, int capacity) : VertexAttribute<float>(name, VertexAttributeType::Float, capacity) { FN("VertexAttributeFloat::VertexAttributeFloat"); }

} // namespace bow