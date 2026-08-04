#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>

#include <d3d11.h>
#include <wrl/client.h>

namespace bow
{

//! A 2D texture and the shader resource view a shader reads it through.
/*!
    DirectX 11 separates the resource from the view onto it. Both are created
    together here, because every texture this backend hands out is meant to be
    sampled.
*/
class D3D11Texture2D : public ITexture2D
{
  public:
    D3D11Texture2D(ID3D11Device *device, ID3D11DeviceContext *context, Texture2DDescription description);
    ~D3D11Texture2D();

    void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    void VCopyFromSystemMemory(void *bitmapInSystemMemory, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    void VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    std::shared_ptr<void> VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;

    Texture2DDescription VGetDescription() override;

    ID3D11ShaderResourceView *GetShaderResourceView() const { return m_shaderResourceView.Get(); }
    ID3D11Texture2D *GetHandle() const { return m_texture.Get(); }

  private:
    D3D11Texture2D(const D3D11Texture2D &) = delete;
    D3D11Texture2D &operator=(const D3D11Texture2D &) = delete;

    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

    Texture2DDescription m_description;
};

typedef std::shared_ptr<D3D11Texture2D> D3D11Texture2DPtr;

} // namespace bow
