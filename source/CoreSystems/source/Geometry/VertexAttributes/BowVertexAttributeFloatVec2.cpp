#pragma once
#include "CoreSystems/Geometry/VertexAttributes/BowVertexAttributeFloatVec2.h"



namespace bow
{

VertexAttributeFloatVec2::VertexAttributeFloatVec2(const std::string &name) : VertexAttribute<Vector2<float>>(name, VertexAttributeType::FloatVector2) { FN("VertexAttributeFloatVec2::VertexAttributeFloatVec2"); }

VertexAttributeFloatVec2::VertexAttributeFloatVec2(const std::string &name, int capacity) : VertexAttribute<Vector2<float>>(name, VertexAttributeType::FloatVector2, capacity) { FN("VertexAttributeFloatVec2::VertexAttributeFloatVec2"); }

} // namespace bow