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

			const std::list<unsigned short> Values;
		};

	}
}