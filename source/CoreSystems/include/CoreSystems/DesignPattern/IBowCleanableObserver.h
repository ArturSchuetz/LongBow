#pragma once
#include <CoreSystems/BowCorePredeclares.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{
class ICleanable
{
  public:
    virtual ~ICleanable() { FN("ICleanable::~ICleanable"); }

    virtual void Clean() = 0;
};

class ICleanableObserver
{
  public:
    virtual ~ICleanableObserver() { FN("ICleanableObserver::~ICleanableObserver"); }

    virtual void NotifyDirty(ICleanable *obj) = 0;
};
} // namespace bow
