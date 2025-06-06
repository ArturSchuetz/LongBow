#pragma once
#include "BowIndicesUnsignedInt.h"

namespace bow {
	
	IndicesUnsignedInt::IndicesUnsignedInt() : IIndicesBase(IndicesType::UnsignedInt), Values(std::vector<unsigned int>())
	{
	}

	IndicesUnsignedInt::IndicesUnsignedInt(int capacity) : IIndicesBase(IndicesType::UnsignedInt), Values(std::vector<unsigned int>(capacity))
	{
	}

	unsigned int IndicesUnsignedInt::Size()
	{
		return Values.size();
	}

	IndicesUnsignedInt::~IndicesUnsignedInt()
	{
		Values.clear();
	}

}