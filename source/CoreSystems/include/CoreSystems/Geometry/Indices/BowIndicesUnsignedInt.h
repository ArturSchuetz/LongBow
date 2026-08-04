#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/Indices/IBowIndicesBase.h"

#include <vector>

namespace bow
{

struct CORESYSTEMS_API IndicesUnsignedInt : IIndicesBase
{
  public:
    IndicesUnsignedInt();
    IndicesUnsignedInt(int capacity);
    ~IndicesUnsignedInt();

    uint32_t Size();
    std::vector<uint32_t> Values;
};

} // namespace bow
