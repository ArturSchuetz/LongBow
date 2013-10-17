#pragma once
#include "LongBow.h"
#include "ICleanable.h"

namespace Bow {

	class ICleanableObserver
    {
	public:
        virtual void NotifyDirty(ICleanable* value) = 0;
    };

	typedef std::shared_ptr<ICleanableObserver> ICleanableObserverPtr;

}