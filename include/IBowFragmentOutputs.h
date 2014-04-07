#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer{

		class IFragmentOutputs
		{
		public:
			virtual ~IFragmentOutputs(){}
			virtual int operator[](std::string index) const = 0;
		};

	}
}