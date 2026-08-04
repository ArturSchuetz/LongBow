#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IFramebuffer
{
  public:
    virtual ~IFramebuffer() {}

    virtual Texture2DPtr VGetColorAttachment(uint32_t OutputLocation) const = 0;
    virtual void VSetColorAttachment(uint32_t OutputLocation, Texture2DPtr texture) = 0;

    virtual Texture2DPtr VGetDepthAttachment() const = 0;
    virtual void VSetDepthAttachment(Texture2DPtr texture) = 0;

    virtual Texture2DPtr VGetDepthStencilAttachment() const = 0;
    virtual void VSetDepthStencilAttachment(Texture2DPtr texture) = 0;
};

} // namespace bow
