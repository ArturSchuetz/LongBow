/**
 * @file BowIndicesUnsignedInt.h
 * @brief Declarations for BowIndicesUnsignedInt.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"

#include <list>

namespace bow {

	struct IndicesUnsignedInt : IIndicesBase
	{
	public:
		IndicesUnsignedInt();
		IndicesUnsignedInt(int capacity);
		~IndicesUnsignedInt();

		unsigned int Size();
		std::vector<unsigned int> Values;
	};

}
