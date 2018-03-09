#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace bow
{
	class ICleanable
	{
	public:
		virtual ~ICleanable(){}

		virtual void Clean() = 0;
	};

	class ICleanableObserver
	{
	public:
		virtual ~ICleanableObserver(){}

		virtual void NotifyDirty(ICleanable* obj) = 0;
	};
}
