#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h"
#include "CoreSystems/Math/BowVector3.h"

#include <string>

namespace bow
{

struct CORESYSTEMS_API VertexAttributeFloatVec3 : VertexAttribute<Vector3<float>>
{
  public:
    VertexAttributeFloatVec3(const std::string &name);
    VertexAttributeFloatVec3(const std::string &name, int capacity);
};

} // namespace bow
