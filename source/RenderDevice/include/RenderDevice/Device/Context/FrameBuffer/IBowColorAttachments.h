#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IColorAttachments
{
  public:
    virtual ~IColorAttachments() {}

    virtual Texture2DPtr VGetAttachment(uint32_t index) const = 0;
    virtual void VSetAttachment(uint32_t index, Texture2DPtr texture) = 0;
};

} // namespace bow
