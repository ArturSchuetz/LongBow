#pragma once
#include "LongBow.h"
#include "IBowIndicesBase.h"

namespace Bow {

	struct IndicesUnsignedInt : IIndicesBase
    {
	public:
		IndicesUnsignedInt();
        IndicesUnsignedInt(int capacity);
		~IndicesUnsignedInt();

		const std::list<unsigned int> Values;
    };

}