#include <DirectX11RenderDevice/Device/BowD3D11RenderContext.h>

#include <DirectX11RenderDevice/BowD3D11RenderDevice.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/Geometry/BowMeshAttribute.h>

#include <RenderDevice/BowRenderState.h>

#include <optick.h>

#include <RenderDevice/BowClearState.h>

#include <dxgi1_2.h>

namespace bow
{

D3D11RenderContext::D3D11RenderContext(HWND windowHandle, uint32_t width, uint32_t height) : m_windowHandle(windowHandle), m_viewport(0, 0, (int)width, (int)height), m_initialized(false)
{
    FN("D3D11RenderContext::D3D11RenderContext");
}

D3D11RenderContext::~D3D11RenderContext()
{
    FN("D3D11RenderContext::~D3D11RenderContext");

    VRelease();
}

bool D3D11RenderContext::Initialize(D3D11RenderDevice *device)
{
    FN("D3D11RenderContext::Initialize");

    if (device == nullptr)
    {
        LOG_ERROR("No device to create a context on.");
        return false;
    }

    m_device = device->GetDevice();
    m_deviceContext = device->GetDeviceContext();

    // FLIP_DISCARD is the presentation model current Windows composes without
    // an extra copy; the older BITBLT models are emulated on top of it.
    DXGI_SWAP_CHAIN_DESC1 swapChainDescription = {};
    swapChainDescription.Width = (UINT)m_viewport.width;
    swapChainDescription.Height = (UINT)m_viewport.height;
    swapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.BufferCount = 2;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescription.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    IDXGIFactory2 *factory = device->GetFactory();
    if (factory == nullptr)
    {
        LOG_ERROR("No DXGI factory to create a swap chain with.");
        return false;
    }

    HRESULT result = factory->CreateSwapChainForHwnd(m_device.Get(), m_windowHandle, &swapChainDescription, nullptr, nullptr, &m_swapChain);
    if (FAILED(result))
    {
        LOG_ERROR("CreateSwapChainForHwnd failed (0x%08X).", (unsigned)result);
        return false;
    }

    // The engine handles fullscreen itself; letting DXGI grab Alt+Enter would
    // change the swap chain behind the application's back.
    factory->MakeWindowAssociation(m_windowHandle, DXGI_MWA_NO_ALT_ENTER);

    if (!CreateBackBufferViews())
    {
        return false;
    }

    VSetViewport(m_viewport);

    m_initialized = true;
    LOG_TRACE("DirectX11 render context successfully initialized!");
    return true;
}

bool D3D11RenderContext::CreateBackBufferViews()
{
    FN("D3D11RenderContext::CreateBackBufferViews");

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT result = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(result))
    {
        LOG_ERROR("Could not get the swap chain's back buffer (0x%08X).", (unsigned)result);
        return false;
    }

    result = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_backBufferView);
    if (FAILED(result))
    {
        LOG_ERROR("CreateRenderTargetView failed (0x%08X).", (unsigned)result);
        return false;
    }

    D3D11_TEXTURE2D_DESC depthDescription = {};
    depthDescription.Width = (UINT)m_viewport.width;
    depthDescription.Height = (UINT)m_viewport.height;
    depthDescription.MipLevels = 1;
    depthDescription.ArraySize = 1;
    depthDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDescription.SampleDesc.Count = 1;
    depthDescription.Usage = D3D11_USAGE_DEFAULT;
    depthDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    result = m_device->CreateTexture2D(&depthDescription, nullptr, &m_depthStencilBuffer);
    if (FAILED(result))
    {
        LOG_ERROR("Could not create the depth stencil buffer (0x%08X).", (unsigned)result);
        return false;
    }

    result = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, &m_depthStencilView);
    if (FAILED(result))
    {
        LOG_ERROR("CreateDepthStencilView failed (0x%08X).", (unsigned)result);
        return false;
    }

    ID3D11RenderTargetView *renderTargets[] = {m_backBufferView.Get()};
    m_deviceContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());

    return true;
}

void D3D11RenderContext::ReleaseBackBufferViews()
{
    FN("D3D11RenderContext::ReleaseBackBufferViews");

    // The swap chain cannot be resized while anything still references its
    // buffers, and the context holds a reference through OMSetRenderTargets.
    m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    m_backBufferView.Reset();
    m_depthStencilView.Reset();
    m_depthStencilBuffer.Reset();
}

void D3D11RenderContext::Resize(uint32_t width, uint32_t height)
{
    FN("D3D11RenderContext::Resize");

    if (!m_initialized || width == 0 || height == 0)
    {
        return;
    }

    ReleaseBackBufferViews();

    HRESULT result = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(result))
    {
        LOG_ERROR("ResizeBuffers failed (0x%08X).", (unsigned)result);
        return;
    }

    m_viewport = Viewport(0, 0, (int)width, (int)height);
    CreateBackBufferViews();
    VSetViewport(m_viewport);
}

void D3D11RenderContext::VRelease()
{
    FN("D3D11RenderContext::VRelease");

    if (!m_initialized)
    {
        return;
    }

    ReleaseBackBufferViews();
    m_swapChain.Reset();
    m_deviceContext.Reset();
    m_device.Reset();

    m_initialized = false;
    LOG_TRACE("D3D11RenderContext released");
}

void D3D11RenderContext::VClear(ClearState clearState)
{
    FN("D3D11RenderContext::VClear");
    OPTICK_EVENT();

    if (m_backBufferView == nullptr)
    {
        return;
    }

    LOG_TRACE("ClearRenderTargetView");
    const float color[4] = {clearState.color.a[0], clearState.color.a[1], clearState.color.a[2], clearState.color.a[3]};
    m_deviceContext->ClearRenderTargetView(m_backBufferView.Get(), color);

    if (m_depthStencilView != nullptr)
    {
        m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, (float)clearState.depth, (UINT8)clearState.stencil);
    }
}

void D3D11RenderContext::VSetViewport(Viewport viewport)
{
    FN("D3D11RenderContext::VSetViewport");

    m_viewport = viewport;

    D3D11_VIEWPORT d3dViewport = {};
    d3dViewport.TopLeftX = (FLOAT)viewport.x;
    d3dViewport.TopLeftY = (FLOAT)viewport.y;
    d3dViewport.Width = (FLOAT)viewport.width;
    d3dViewport.Height = (FLOAT)viewport.height;
    d3dViewport.MinDepth = 0.0f;
    d3dViewport.MaxDepth = 1.0f;

    m_deviceContext->RSSetViewports(1, &d3dViewport);
}

Viewport D3D11RenderContext::VGetViewport()
{
    FN("D3D11RenderContext::VGetViewport");

    return m_viewport;
}

void D3D11RenderContext::VSwapBuffers(bool vsync)
{
    FN("D3D11RenderContext::VSwapBuffers");
    OPTICK_EVENT();

    if (m_swapChain == nullptr)
    {
        return;
    }

    // FLIP_DISCARD requires DXGI_PRESENT_ALLOW_TEARING to be absent when a
    // sync interval is used, so the two cases are kept apart.
    LOG_TRACE("Present");
    const HRESULT result = vsync ? m_swapChain->Present(1, 0) : m_swapChain->Present(0, 0);
    if (FAILED(result))
    {
        LOG_ERROR("Present failed (0x%08X).", (unsigned)result);
        return;
    }

    // Present unbinds the render target under the flip model.
    ID3D11RenderTargetView *renderTargets[] = {m_backBufferView.Get()};
    m_deviceContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());
}

void D3D11RenderContext::VBeginFrame() {}

void D3D11RenderContext::VEndFrame() {}

//
// Everything below waits on the resource types this backend does not have yet.
// They report rather than returning silently, so a caller learns which piece
// is missing instead of finding a null pointer later.
//

VertexAttributeBindingsPtr D3D11RenderContext::VCreateVertexAttributeBindings(MeshAttribute /*mesh*/, ShaderVertexAttributeMap /*shaderAttributes*/, BufferHint /*usageHint*/)
{
    FN("D3D11RenderContext::VCreateVertexAttributeBindings");

    LOG_ERROR("DirectX 11: vertex attribute bindings are not implemented yet.");
    return nullptr;
}

VertexAttributeBindingsPtr D3D11RenderContext::VCreateVertexAttributeBindings(MeshBufferPtr /*meshBuffers*/)
{
    FN("D3D11RenderContext::VCreateVertexAttributeBindings");

    LOG_ERROR("DirectX 11: vertex attribute bindings are not implemented yet.");
    return nullptr;
}

VertexAttributeBindingsPtr D3D11RenderContext::VCreateVertexAttributeBindings()
{
    FN("D3D11RenderContext::VCreateVertexAttributeBindings");

    LOG_ERROR("DirectX 11: vertex attribute bindings are not implemented yet.");
    return nullptr;
}

FramebufferPtr D3D11RenderContext::VCreateFramebuffer()
{
    FN("D3D11RenderContext::VCreateFramebuffer");

    LOG_ERROR("DirectX 11: framebuffers are not implemented yet.");
    return nullptr;
}

void D3D11RenderContext::VDraw(PrimitiveType /*primitiveType*/, VertexAttributeBindingsPtr /*vertexAttributeBindings*/, ShaderProgramPtr /*shaderProgram*/, RenderState /*renderState*/)
{
    FN("D3D11RenderContext::VDraw");

    LOG_ERROR("DirectX 11: drawing is not implemented yet.");
}

void D3D11RenderContext::VDraw(PrimitiveType /*primitiveType*/, uint32_t /*offset*/, uint32_t /*count*/, VertexAttributeBindingsPtr /*vertexAttributeBindings*/, ShaderProgramPtr /*shaderProgram*/, RenderState /*renderState*/)
{
    FN("D3D11RenderContext::VDraw");

    LOG_ERROR("DirectX 11: drawing is not implemented yet.");
}

void D3D11RenderContext::VDraw(PrimitiveType /*primitiveType*/, uint32_t /*offset*/, uint32_t /*count*/, VertexAttributeBindingsPtr /*vertexAttributeBindings*/, ShaderResourceBindingsPtr /*shaderResourceBindings*/, ShaderProgramPtr /*shaderProgram*/,
                               RenderState /*renderState*/)
{
    FN("D3D11RenderContext::VDraw");

    LOG_ERROR("DirectX 11: drawing is not implemented yet.");
}

void D3D11RenderContext::VDrawLine(const bow::Vector3<float> & /*start*/, const bow::Vector3<float> & /*end*/)
{
    FN("D3D11RenderContext::VDrawLine");

    LOG_ERROR("DirectX 11: drawing is not implemented yet.");
}

void D3D11RenderContext::VSetFramebuffer(FramebufferPtr /*framebufer*/)
{
    FN("D3D11RenderContext::VSetFramebuffer");

    LOG_ERROR("DirectX 11: framebuffers are not implemented yet.");
}

void D3D11RenderContext::VTraceRays(void * /*shaderProgram*/, ShaderResourceBindingsPtr /*resourceBindings*/, Texture2DPtr /*outputImage*/, uint32_t /*width*/, uint32_t /*height*/)
{
    FN("D3D11RenderContext::VTraceRays");

    LOG_ERROR("Ray tracing is not supported in DirectX 11.");
}

} // namespace bow
