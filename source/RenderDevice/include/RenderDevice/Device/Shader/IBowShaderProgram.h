#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <CoreSystems/BowMath.h>

namespace bow
{

class IShaderProgram
{
  public:
    virtual ~IShaderProgram() {}

    virtual ShaderVertexAttributePtr VGetVertexAttribute(std::string name) = 0;
    virtual ShaderVertexAttributeMap VGetVertexAttributes() = 0;
    virtual int VGetFragmentOutputLocation(std::string name) = 0;

    virtual ShaderResourceBindingsPtr VCreateResourceBindingObjects() = 0;

    virtual void VSetPushConstants(const char *name, const void *data, size_t offset, size_t size) = 0;
    virtual void VSetPushConstants(ShaderStage shaderStage, const void *data, size_t offset, size_t size) = 0;
};

} // namespace bow
