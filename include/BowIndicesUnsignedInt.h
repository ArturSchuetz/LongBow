#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"

#include <list>

namespace Bow {
	namespace Core {

		struct IndicesUnsignedInt : IIndicesBase
		{
		public:
			IndicesUnsignedInt();
			IndicesUnsignedInt(int capacity);
			~IndicesUnsignedInt();

			std::vector<unsigned int> Values;
		};

	}
}