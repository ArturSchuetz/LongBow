#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Context/FrameBuffer/IBowFramebuffer.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <map>
#include <vector>

namespace bow
{

typedef std::shared_ptr<class D3D11Texture2D> D3D11Texture2DPtr;

//! A set of textures to render into.
/*!
    OpenGL has a framebuffer object that attachments are hung on; DirectX has
    no such object at all -- render targets are passed to OMSetRenderTargets as
    a list. This class is therefore only the list plus the views onto the
    textures, built when an attachment changes rather than on every bind.
*/
class D3D11Framebuffer : public IFramebuffer
{
  public:
    explicit D3D11Framebuffer(ID3D11Device *device);
    ~D3D11Framebuffer();

    Texture2DPtr VGetColorAttachment(uint32_t OutputLocation) const override;
    void VSetColorAttachment(uint32_t OutputLocation, Texture2DPtr texture) override;

    Texture2DPtr VGetDepthAttachment() const override;
    void VSetDepthAttachment(Texture2DPtr texture) override;

    Texture2DPtr VGetDepthStencilAttachment() const override;
    void VSetDepthStencilAttachment(Texture2DPtr texture) override;

    //! Render target views in slot order, rebuilding them if needed.
    const std::vector<ID3D11RenderTargetView *> &GetRenderTargets();

    ID3D11DepthStencilView *GetDepthStencilView();

  private:
    D3D11Framebuffer(const D3D11Framebuffer &) = delete;
    D3D11Framebuffer &operator=(const D3D11Framebuffer &) = delete;

    void Rebuild();

    ID3D11Device *m_device;

    // Ordered, so that the render target list comes out in slot order.
    std::map<uint32_t, D3D11Texture2DPtr> m_colorAttachments;
    D3D11Texture2DPtr m_depthAttachment;

    std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_colorViews;
    std::vector<ID3D11RenderTargetView *> m_renderTargets;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthView;

    bool m_dirty;
};

typedef std::shared_ptr<D3D11Framebuffer> D3D11FramebufferPtr;

} // namespace bow
