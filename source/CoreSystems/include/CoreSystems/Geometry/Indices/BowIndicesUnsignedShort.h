#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Geometry/Indices/IBowIndicesBase.h"

#include <vector>

namespace bow
{

struct CORESYSTEMS_API IndicesUnsignedShort : IIndicesBase
{
  public:
    IndicesUnsignedShort();
    IndicesUnsignedShort(int capacity);
    ~IndicesUnsignedShort();

    uint32_t Size();
    std::vector<uint16_t> Values;
};

} // namespace bow
