#pragma once
#include "BowIndicesUnsignedInt.h"

namespace Bow {
	namespace Core {

		IndicesUnsignedInt::IndicesUnsignedInt() : IIndicesBase(IndicesType::UnsignedInt), Values(std::list<unsigned int>())
		{
		}

		IndicesUnsignedInt::IndicesUnsignedInt(int capacity) : IIndicesBase(IndicesType::UnsignedInt), Values(std::list<unsigned int>(capacity))
		{
		}

		IndicesUnsignedInt::~IndicesUnsignedInt()
		{
		}

	}
}