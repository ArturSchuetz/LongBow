#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class ITopLevelAccelerationStructure
{
  public:
    virtual ~ITopLevelAccelerationStructure() = 0;
};

inline ITopLevelAccelerationStructure::~ITopLevelAccelerationStructure() {}

} // namespace bow