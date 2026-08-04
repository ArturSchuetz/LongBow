#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IFragmentOutputs
{
  public:
    virtual ~IFragmentOutputs() {}
    virtual int operator[](std::string name) const = 0;
};

} // namespace bow
