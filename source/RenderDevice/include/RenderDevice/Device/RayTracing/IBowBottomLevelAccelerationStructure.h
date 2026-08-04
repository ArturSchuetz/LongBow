#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IBottomLevelAccelerationStructure
{
  public:
    virtual ~IBottomLevelAccelerationStructure() = 0;
};

inline IBottomLevelAccelerationStructure::~IBottomLevelAccelerationStructure() {}

} // namespace bow