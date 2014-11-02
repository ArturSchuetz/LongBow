#pragma once
#include "BowIndicesUnsignedShort.h"

namespace Bow {
	namespace Core {

		IndicesUnsignedShort::IndicesUnsignedShort() :IIndicesBase(IndicesType::UnsignedShort), Values(std::vector<unsigned short>())
		{
		}

		IndicesUnsignedShort::IndicesUnsignedShort(int capacity) : IIndicesBase(IndicesType::UnsignedShort), Values(std::vector<unsigned short>(capacity))
		{
		}

		IndicesUnsignedShort::~IndicesUnsignedShort()
		{
			Values.clear();
		}

	}
}