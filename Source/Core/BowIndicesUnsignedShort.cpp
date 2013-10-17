#pragma once
#include "BowIndicesUnsignedShort.h"

namespace Bow {

	IndicesUnsignedShort::IndicesUnsignedShort() : IIndicesBase(IndicesType::UnsignedInt), Values(std::list<unsigned short>())
    {
    }

    IndicesUnsignedShort::IndicesUnsignedShort(int capacity) : IIndicesBase(IndicesType::UnsignedInt), Values(std::list<unsigned short>(capacity))
    {
    }

	IndicesUnsignedShort::~IndicesUnsignedShort()
	{
	}
}