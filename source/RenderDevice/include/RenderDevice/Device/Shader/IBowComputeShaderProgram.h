#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <CoreSystems/BowMath.h>

namespace bow
{

class IComputeShaderProgram
{
  public:
    virtual ~IComputeShaderProgram() {}

    virtual ShaderResourceBindingsPtr VCreateComputeResourceBindingObjects() = 0;

    virtual void VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
};

} // namespace bow
