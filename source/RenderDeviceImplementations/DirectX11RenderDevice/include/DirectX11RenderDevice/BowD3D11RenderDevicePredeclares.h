#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>

#include <memory>

namespace bow
{

class D3D11RenderDevice;
class D3D11GraphicsWindow;
class D3D11RenderContext;

typedef std::shared_ptr<D3D11GraphicsWindow> D3D11GraphicsWindowPtr;
typedef std::shared_ptr<D3D11RenderContext> D3D11RenderContextPtr;

} // namespace bow
