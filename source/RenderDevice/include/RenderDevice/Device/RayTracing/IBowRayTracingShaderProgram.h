#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

enum class RTDescriptorType : uint8_t
{
    AccelerationStructure,
    StorageImage,
    UniformBuffer,
    StorageBuffer,
    CombinedImageSampler
};

class IRayTracingShaderProgram
{
  public:
    virtual ~IRayTracingShaderProgram() = 0;

    virtual void VAddDescriptorBinding(uint32_t set, uint32_t binding, const std::string &name, RTDescriptorType type) = 0;
    virtual bool VBuild() = 0;
    virtual ShaderResourceBindingsPtr VCreateResourceBindingObjects() = 0;
};

inline IRayTracingShaderProgram::~IRayTracingShaderProgram() {}

} // namespace bow