#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IGraphicsWindow
{
  public:
    virtual ~IGraphicsWindow() {};
    virtual void VRelease() = 0;

    virtual RenderContextPtr VGetContext() const = 0;
    virtual void VPollWindowEvents() const = 0;

    virtual void *VGetHandle() const = 0;
    virtual void VSetWindowTitle(const char *title) const = 0;

    virtual void VHideCursor() const = 0;
    virtual void VShowCursor() const = 0;

    virtual int VGetWidth() const = 0;
    virtual int VGetHeight() const = 0;

    virtual bool VIsVisible() const = 0;
    virtual bool VIsFocused() const = 0;
    virtual bool VShouldClose() const = 0;
};

} // namespace bow
