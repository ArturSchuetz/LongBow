#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	class IFragmentOutputs
	{
	public:
		virtual ~IFragmentOutputs(){}
		virtual int operator[](std::string index) const = 0;
	};

}
