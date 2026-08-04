#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IShaderResourceBindings
{
  public:
    virtual ~IShaderResourceBindings() {}

    virtual void VSetBuffer(const char *name, UniformBufferPtr uniformBuffer) = 0;
    virtual void VSetBuffer(const char *name, StorageBufferPtr storageBuffer) = 0;
    virtual void VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler) = 0;
    virtual void VSetAccelerationStructure(const char *name, void *accelerationStructure) = 0;
    virtual void VSetStorageImage(const char *name, Texture2DPtr texture) = 0;
};

} // namespace bow
