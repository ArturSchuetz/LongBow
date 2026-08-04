#pragma once
#include "CoreSystems/Geometry/VertexAttributes/BowVertexAttributeFloatVec3.h"



namespace bow
{

VertexAttributeFloatVec3::VertexAttributeFloatVec3(const std::string &name) : VertexAttribute<Vector3<float>>(name, VertexAttributeType::FloatVector3) { FN("VertexAttributeFloatVec3::VertexAttributeFloatVec3"); }

VertexAttributeFloatVec3::VertexAttributeFloatVec3(const std::string &name, int capacity) : VertexAttribute<Vector3<float>>(name, VertexAttributeType::FloatVector3, capacity) { FN("VertexAttributeFloatVec3::VertexAttributeFloatVec3"); }

} // namespace bow