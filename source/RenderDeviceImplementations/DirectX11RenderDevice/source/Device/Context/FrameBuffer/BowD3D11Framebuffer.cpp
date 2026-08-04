#include <DirectX11RenderDevice/Device/Context/FrameBuffer/BowD3D11Framebuffer.h>

#include <DirectX11RenderDevice/Device/Textures/BowD3D11Texture2D.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11Framebuffer::D3D11Framebuffer(ID3D11Device *device) : m_device(device), m_dirty(true) { FN("D3D11Framebuffer::D3D11Framebuffer"); }

D3D11Framebuffer::~D3D11Framebuffer() { FN("D3D11Framebuffer::~D3D11Framebuffer"); }

Texture2DPtr D3D11Framebuffer::VGetColorAttachment(uint32_t OutputLocation) const
{
    FN("D3D11Framebuffer::VGetColorAttachment");

    std::map<uint32_t, D3D11Texture2DPtr>::const_iterator it = m_colorAttachments.find(OutputLocation);
    return (it != m_colorAttachments.end()) ? it->second : nullptr;
}

void D3D11Framebuffer::VSetColorAttachment(uint32_t OutputLocation, Texture2DPtr texture)
{
    FN("D3D11Framebuffer::VSetColorAttachment");

    if (texture == nullptr)
    {
        m_colorAttachments.erase(OutputLocation);
        m_dirty = true;
        return;
    }

    D3D11Texture2DPtr d3dTexture = std::dynamic_pointer_cast<D3D11Texture2D>(texture);
    if (d3dTexture == nullptr)
    {
        LOG_ERROR("The texture attached at slot %u was not created by the DirectX 11 device.", OutputLocation);
        return;
    }

    m_colorAttachments[OutputLocation] = d3dTexture;
    m_dirty = true;
}

Texture2DPtr D3D11Framebuffer::VGetDepthAttachment() const
{
    FN("D3D11Framebuffer::VGetDepthAttachment");

    return m_depthAttachment;
}

void D3D11Framebuffer::VSetDepthAttachment(Texture2DPtr texture)
{
    FN("D3D11Framebuffer::VSetDepthAttachment");

    m_depthAttachment = std::dynamic_pointer_cast<D3D11Texture2D>(texture);
    m_dirty = true;
}

Texture2DPtr D3D11Framebuffer::VGetDepthStencilAttachment() const
{
    FN("D3D11Framebuffer::VGetDepthStencilAttachment");

    // DirectX has one depth-stencil slot rather than separate depth and
    // depth-stencil attachments, so both names lead to the same texture.
    return m_depthAttachment;
}

void D3D11Framebuffer::VSetDepthStencilAttachment(Texture2DPtr texture)
{
    FN("D3D11Framebuffer::VSetDepthStencilAttachment");

    VSetDepthAttachment(texture);
}

void D3D11Framebuffer::Rebuild()
{
    FN("D3D11Framebuffer::Rebuild");

    m_colorViews.clear();
    m_renderTargets.clear();
    m_depthView.Reset();

    for (std::map<uint32_t, D3D11Texture2DPtr>::const_iterator it = m_colorAttachments.begin(); it != m_colorAttachments.end(); ++it)
    {
        if (it->second == nullptr || it->second->GetHandle() == nullptr)
        {
            continue;
        }

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> view;
        const HRESULT result = m_device->CreateRenderTargetView(it->second->GetHandle(), nullptr, &view);
        if (FAILED(result))
        {
            // A texture created only as a shader resource cannot be rendered
            // into; that is a bind-flag mismatch rather than a transient error.
            LOG_ERROR("Could not create a render target view for attachment %u (0x%08X). Was the texture created as a render target?", it->first, (unsigned)result);
            continue;
        }

        m_renderTargets.push_back(view.Get());
        m_colorViews.push_back(view);
    }

    if (m_depthAttachment != nullptr && m_depthAttachment->GetHandle() != nullptr)
    {
        const HRESULT result = m_device->CreateDepthStencilView(m_depthAttachment->GetHandle(), nullptr, &m_depthView);
        if (FAILED(result))
        {
            LOG_ERROR("Could not create a depth stencil view for the attached texture (0x%08X).", (unsigned)result);
        }
    }

    m_dirty = false;
}

const std::vector<ID3D11RenderTargetView *> &D3D11Framebuffer::GetRenderTargets()
{
    FN("D3D11Framebuffer::GetRenderTargets");

    if (m_dirty)
    {
        Rebuild();
    }
    return m_renderTargets;
}

ID3D11DepthStencilView *D3D11Framebuffer::GetDepthStencilView()
{
    FN("D3D11Framebuffer::GetDepthStencilView");

    if (m_dirty)
    {
        Rebuild();
    }
    return m_depthView.Get();
}

} // namespace bow
