#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h"
#include "CoreSystems/Math/BowVector4.h"

#include <string>

namespace bow
{

struct CORESYSTEMS_API VertexAttributeFloatVec4 : VertexAttribute<Vector4<float>>
{
  public:
    VertexAttributeFloatVec4(const std::string &name);
    VertexAttributeFloatVec4(const std::string &name, int capacity);
};

} // namespace bow
