#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

#include <d3d11.h>
#include <wrl/client.h>

namespace bow
{

//! Filtering and addressing, as a state object.
/*!
    DirectX 11 wants sampler state as an object created up front, where OpenGL
    takes individual parameters. The object is created in the constructor so
    that binding it later is only a pointer assignment.
*/
class D3D11TextureSampler : public ITextureSampler
{
  public:
    D3D11TextureSampler(ID3D11Device *device, TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy);
    ~D3D11TextureSampler();

    ID3D11SamplerState *GetHandle() const { return m_sampler.Get(); }

  private:
    D3D11TextureSampler(const D3D11TextureSampler &) = delete;
    D3D11TextureSampler &operator=(const D3D11TextureSampler &) = delete;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
};

typedef std::shared_ptr<D3D11TextureSampler> D3D11TextureSamplerPtr;

} // namespace bow
