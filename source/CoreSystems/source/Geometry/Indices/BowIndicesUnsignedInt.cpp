#pragma once
#include "CoreSystems/Geometry/Indices/BowIndicesUnsignedInt.h"

#include <CoreSystems/BowLogger.h>

namespace bow
{

IndicesUnsignedInt::IndicesUnsignedInt() : IIndicesBase(IndicesType::UnsignedInt), Values(std::vector<uint32_t>()) { FN("IndicesUnsignedInt::IndicesUnsignedInt"); }

IndicesUnsignedInt::IndicesUnsignedInt(int capacity) : IIndicesBase(IndicesType::UnsignedInt), Values(std::vector<uint32_t>(capacity)) { FN("IndicesUnsignedInt::IndicesUnsignedInt"); }

uint32_t IndicesUnsignedInt::Size()
{
    FN("IndicesUnsignedInt::Size");

    return (uint32_t)Values.size();
}

IndicesUnsignedInt::~IndicesUnsignedInt()
{
    FN("IndicesUnsignedInt::~IndicesUnsignedInt");

    Values.clear();
}

} // namespace bow