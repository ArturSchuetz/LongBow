#pragma once
#include "CoreSystems/Geometry/Indices/BowIndicesUnsignedShort.h"

#include <CoreSystems/BowLogger.h>

namespace bow
{

IndicesUnsignedShort::IndicesUnsignedShort() : IIndicesBase(IndicesType::UnsignedShort), Values(std::vector<uint16_t>()) { FN("IndicesUnsignedShort::IndicesUnsignedShort"); }

IndicesUnsignedShort::IndicesUnsignedShort(int capacity) : IIndicesBase(IndicesType::UnsignedShort), Values(std::vector<uint16_t>(capacity)) { FN("IndicesUnsignedShort::IndicesUnsignedShort"); }

uint32_t IndicesUnsignedShort::Size()
{
    FN("IndicesUnsignedShort::Size");

    return (uint32_t)Values.size();
}

IndicesUnsignedShort::~IndicesUnsignedShort()
{
    FN("IndicesUnsignedShort::~IndicesUnsignedShort");

    Values.clear();
}

} // namespace bow
