#include <DirectX11RenderDevice/Device/BowD3D11RenderContext.h>

#include <DirectX11RenderDevice/BowD3D11RenderDevice.h>
#include <DirectX11RenderDevice/BowD3D11TypeConverter.h>
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11IndexBuffer.h>
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11UniformBuffer.h>
#include <DirectX11RenderDevice/Device/Context/VertexAttributeBindings/BowD3D11VertexAttributeBindings.h>
#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderProgram.h>
#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderResourceBindings.h>
#include <DirectX11RenderDevice/Device/Textures/BowD3D11Texture2D.h>
#include <DirectX11RenderDevice/Device/Textures/BowD3D11TextureSampler.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/Geometry/BowMeshAttribute.h>

#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/Context/Mesh/BowMeshBuffers.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>

#include <optick.h>

#include <RenderDevice/BowClearState.h>

#include <dxgi1_2.h>

namespace bow
{

D3D11RenderContext::D3D11RenderContext(HWND windowHandle, uint32_t width, uint32_t height) : m_windowHandle(windowHandle), m_viewport(0, 0, (int)width, (int)height), m_renderStateKey(0), m_renderStateValid(false), m_initialized(false)
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

VertexAttributeBindingsPtr D3D11RenderContext::VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers)
{
    FN("D3D11RenderContext::VCreateVertexAttributeBindings");

    if (meshBuffers == nullptr)
    {
        return nullptr;
    }

    VertexAttributeBindingsPtr bindings = VCreateVertexAttributeBindings();
    if (meshBuffers->IndexBuffer != nullptr)
    {
        bindings->VSetIndexBuffer(meshBuffers->IndexBuffer);
    }

    VertexBufferAttributeMap attributes = meshBuffers->GetAttributes();
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
    {
        bindings->VSetAttribute(it->first, it->second);
    }

    return bindings;
}

VertexAttributeBindingsPtr D3D11RenderContext::VCreateVertexAttributeBindings()
{
    FN("D3D11RenderContext::VCreateVertexAttributeBindings");

    return D3D11VertexAttributeBindingsPtr(new D3D11VertexAttributeBindings(m_device.Get(), m_deviceContext.Get()));
}

FramebufferPtr D3D11RenderContext::VCreateFramebuffer()
{
    FN("D3D11RenderContext::VCreateFramebuffer");

    LOG_ERROR("DirectX 11: framebuffers are not implemented yet.");
    return nullptr;
}

void D3D11RenderContext::VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("D3D11RenderContext::VDraw");
    OPTICK_EVENT();

    D3D11VertexAttributeBindingsPtr bindings = std::dynamic_pointer_cast<D3D11VertexAttributeBindings>(vertexAttributeBindings);
    const uint32_t count = (bindings != nullptr) ? bindings->GetVertexCount() : 0;

    Draw(primitiveType, 0, count, vertexAttributeBindings, nullptr, shaderProgram, renderState);
}

void D3D11RenderContext::VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("D3D11RenderContext::VDraw");
    OPTICK_EVENT();

    Draw(primitiveType, offset, count, vertexAttributeBindings, nullptr, shaderProgram, renderState);
}

void D3D11RenderContext::VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram,
                               RenderState renderState)
{
    FN("D3D11RenderContext::VDraw");
    OPTICK_EVENT();

    Draw(primitiveType, offset, count, vertexAttributeBindings, shaderResourceBindings, shaderProgram, renderState);
}

void D3D11RenderContext::Draw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("D3D11RenderContext::Draw");

    D3D11ShaderProgramPtr program = std::dynamic_pointer_cast<D3D11ShaderProgram>(shaderProgram);
    D3D11VertexAttributeBindingsPtr bindings = std::dynamic_pointer_cast<D3D11VertexAttributeBindings>(vertexAttributeBindings);

    if (program == nullptr || bindings == nullptr)
    {
        LOG_ERROR("The shader program or the vertex attribute bindings were not created by the DirectX 11 device.");
        return;
    }

    if (count == 0)
    {
        return;
    }

    ApplyRenderState(renderState);

    // The program has to be set before the resource bindings, which resolve
    // their names through its reflection, and before the input layout, which
    // is validated against its vertex signature.
    program->Bind();

    if (!bindings->Bind(program))
    {
        return;
    }

    ApplyShaderResourceBindings(shaderResourceBindings, program);

    LOG_TRACE("IASetPrimitiveTopology");
    m_deviceContext->IASetPrimitiveTopology(D3D11TypeConverter::ToPrimitiveTopology(primitiveType));

    if (bindings->VGetIndexBuffer() != nullptr)
    {
        LOG_TRACE("DrawIndexed");
        m_deviceContext->DrawIndexed(count, offset, 0);
    }
    else
    {
        LOG_TRACE("Draw");
        m_deviceContext->Draw(count, offset);
    }
}

void D3D11RenderContext::ApplyRenderState(const RenderState &renderState)
{
    FN("D3D11RenderContext::ApplyRenderState");

    // A cheap identity for the settings that matter, so the state objects are
    // only rebuilt when something actually changed.
    uint64_t key = 0;
    key |= (uint64_t)(renderState.faceCulling.Enabled ? 1 : 0) << 0;
    key |= (uint64_t)renderState.faceCulling.Face << 1;
    key |= (uint64_t)renderState.faceCulling.FrontFaceWindingOrder << 4;
    key |= (uint64_t)(renderState.depthTest.Enabled ? 1 : 0) << 6;
    key |= (uint64_t)renderState.depthTest.Function << 7;
    key |= (uint64_t)(renderState.depthMask ? 1 : 0) << 11;
    key |= (uint64_t)(renderState.blending.Enabled ? 1 : 0) << 12;
    key |= (uint64_t)renderState.rasterizationMode << 13;

    if (m_renderStateValid && key == m_renderStateKey)
    {
        return;
    }

    D3D11_RASTERIZER_DESC rasterizer = {};
    rasterizer.FillMode = (renderState.rasterizationMode == RasterizationMode::Fill) ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
    if (!renderState.faceCulling.Enabled)
    {
        rasterizer.CullMode = D3D11_CULL_NONE;
    }
    else
    {
        rasterizer.CullMode = (renderState.faceCulling.Face == CullFace::Front) ? D3D11_CULL_FRONT : D3D11_CULL_BACK;
    }
    // SPIRV-Cross flips Y when it translates a GLSL vertex shader, which
    // reverses the winding the rasteriser sees, so the front face is the
    // opposite of what the render state names.
    rasterizer.FrontCounterClockwise = (renderState.faceCulling.FrontFaceWindingOrder == WindingOrder::Clockwise) ? TRUE : FALSE;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.ScissorEnable = renderState.scissorTest.Enabled ? TRUE : FALSE;

    m_rasterizerState.Reset();
    m_device->CreateRasterizerState(&rasterizer, &m_rasterizerState);
    m_deviceContext->RSSetState(m_rasterizerState.Get());

    D3D11_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = renderState.depthTest.Enabled ? TRUE : FALSE;
    depthStencil.DepthWriteMask = renderState.depthMask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    switch (renderState.depthTest.Function)
    {
    case DepthTestFunction::Never:
        depthStencil.DepthFunc = D3D11_COMPARISON_NEVER;
        break;
    case DepthTestFunction::Less:
        depthStencil.DepthFunc = D3D11_COMPARISON_LESS;
        break;
    case DepthTestFunction::Equal:
        depthStencil.DepthFunc = D3D11_COMPARISON_EQUAL;
        break;
    case DepthTestFunction::LessThanOrEqual:
        depthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        break;
    case DepthTestFunction::Greater:
        depthStencil.DepthFunc = D3D11_COMPARISON_GREATER;
        break;
    case DepthTestFunction::NotEqual:
        depthStencil.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
        break;
    case DepthTestFunction::GreaterThanOrEqual:
        depthStencil.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
        break;
    default:
        depthStencil.DepthFunc = D3D11_COMPARISON_ALWAYS;
        break;
    }

    m_depthStencilState.Reset();
    m_device->CreateDepthStencilState(&depthStencil, &m_depthStencilState);
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    D3D11_BLEND_DESC blend = {};
    blend.RenderTarget[0].BlendEnable = renderState.blending.Enabled ? TRUE : FALSE;
    blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_blendState.Reset();
    m_device->CreateBlendState(&blend, &m_blendState);
    m_deviceContext->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);

    m_renderStateKey = key;
    m_renderStateValid = true;
}

void D3D11RenderContext::ApplyShaderResourceBindings(ShaderResourceBindingsPtr shaderResourceBindings, const D3D11ShaderProgramPtr &program)
{
    FN("D3D11RenderContext::ApplyShaderResourceBindings");

    if (shaderResourceBindings == nullptr)
    {
        return;
    }

    D3D11ShaderResourceBindingsPtr bindings = std::dynamic_pointer_cast<D3D11ShaderResourceBindings>(shaderResourceBindings);
    if (bindings == nullptr)
    {
        LOG_ERROR("Resource bindings were not created by the DirectX 11 device.");
        return;
    }

    // The names the caller used are turned into slots here, using what the
    // shader reflection reported. DirectX binds per stage, so a resource the
    // reflection saw in both has to be set twice.
    for (const auto &entry : bindings->GetTextures())
    {
        const D3D11ShaderProgram::ResourceSlot *slot = program->FindTexture(entry.first);
        if (slot == nullptr)
        {
            LOG_WARNING("Shader has no texture named '%s'; the binding is ignored.", entry.first.c_str());
            continue;
        }

        ID3D11ShaderResourceView *views[] = {entry.second.texture->GetShaderResourceView()};
        if (slot->inVertexStage)
        {
            m_deviceContext->VSSetShaderResources(slot->slot, 1, views);
        }
        if (slot->inPixelStage)
        {
            m_deviceContext->PSSetShaderResources(slot->slot, 1, views);
        }

        if (entry.second.sampler != nullptr)
        {
            // SPIRV-Cross names the sampler after the combined image sampler
            // it split, prefixed, so the texture name is tried first and the
            // sampler slot falls back to the texture slot when it matches.
            const D3D11ShaderProgram::ResourceSlot *samplerSlot = program->FindSampler(entry.first);
            const uint32_t index = (samplerSlot != nullptr) ? samplerSlot->slot : slot->slot;

            ID3D11SamplerState *samplers[] = {entry.second.sampler->GetHandle()};
            if (slot->inVertexStage)
            {
                m_deviceContext->VSSetSamplers(index, 1, samplers);
            }
            if (slot->inPixelStage)
            {
                m_deviceContext->PSSetSamplers(index, 1, samplers);
            }
        }
    }

    for (const auto &entry : bindings->GetUniformBuffers())
    {
        const D3D11ShaderProgram::ResourceSlot *slot = program->FindConstantBuffer(entry.first);
        if (slot == nullptr)
        {
            LOG_WARNING("Shader has no constant buffer named '%s'; the binding is ignored.", entry.first.c_str());
            continue;
        }

        ID3D11Buffer *buffers[] = {entry.second->GetHandle()};
        if (slot->inVertexStage)
        {
            m_deviceContext->VSSetConstantBuffers(slot->slot, 1, buffers);
        }
        if (slot->inPixelStage)
        {
            m_deviceContext->PSSetConstantBuffers(slot->slot, 1, buffers);
        }
    }
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
