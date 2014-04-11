#pragma once
#include "BowPrerequisites.h"

namespace Bow
{
	namespace Core
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
}