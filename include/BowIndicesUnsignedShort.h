#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"

#include <list>

namespace Bow {
	namespace Core {

		struct IndicesUnsignedShort : IIndicesBase
		{
		public:
			IndicesUnsignedShort();
			IndicesUnsignedShort(int capacity);
			~IndicesUnsignedShort();

			std::vector<unsigned short> Values;
		};

	}
}