#pragma once
#include "LongBow.h"
#include "IBowIndicesBase.h"

namespace Bow {

	struct IndicesUnsignedShort : IIndicesBase
    {
	public:
		IndicesUnsignedShort();
        IndicesUnsignedShort(int capacity);
		~IndicesUnsignedShort();

		const std::list<unsigned short> Values;
    };

}