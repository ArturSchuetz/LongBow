#pragma once
#include "LongBow.h"

namespace Bow {

	class IFragmentOutputs
	{
	public:
		virtual ~IFragmentOutputs(){}
		virtual int operator[](std::string index) const = 0;
	};

	typedef std::shared_ptr<IFragmentOutputs> FragmentOutputsPtr;

}