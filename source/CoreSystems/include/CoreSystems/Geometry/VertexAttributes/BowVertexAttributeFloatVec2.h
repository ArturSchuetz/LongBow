#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h"
#include "CoreSystems/Math/BowVector2.h"

#include <string>

namespace bow
{

struct CORESYSTEMS_API VertexAttributeFloatVec2 : VertexAttribute<Vector2<float>>
{
  public:
    VertexAttributeFloatVec2(const std::string &name);
    VertexAttributeFloatVec2(const std::string &name, int capacity);
};

} // namespace bow
