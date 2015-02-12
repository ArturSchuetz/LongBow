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

		unsigned int IndicesUnsignedShort::Size()
		{
			return Values.size();
		}

		IndicesUnsignedShort::~IndicesUnsignedShort()
		{
			Values.clear();
		}

	}
}