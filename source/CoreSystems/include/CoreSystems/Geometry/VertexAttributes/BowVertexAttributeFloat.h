#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h"
#include "CoreSystems/Math/BowVector2.h"

#include <string>

namespace bow
{

struct CORESYSTEMS_API VertexAttributeFloat : VertexAttribute<float>
{
  public:
    VertexAttributeFloat(const std::string &name);
    VertexAttributeFloat(const std::string &name, int capacity);
};

} // namespace bow
