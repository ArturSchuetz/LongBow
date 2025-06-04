/**
 * @file BowIndicesUnsignedShort.h
 * @brief Declarations for BowIndicesUnsignedShort.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"

#include <list>

namespace bow {

	struct IndicesUnsignedShort : IIndicesBase
	{
	public:
		IndicesUnsignedShort();
		IndicesUnsignedShort(int capacity);
		~IndicesUnsignedShort();

		unsigned int Size();
		std::vector<unsigned short> Values;
	};

}
