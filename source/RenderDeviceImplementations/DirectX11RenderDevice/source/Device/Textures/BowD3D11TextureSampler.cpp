#include <DirectX11RenderDevice/Device/Textures/BowD3D11TextureSampler.h>

#include <DirectX11RenderDevice/BowD3D11TypeConverter.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11TextureSampler::D3D11TextureSampler(ID3D11Device *device, TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
    : ITextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy)
{
    FN("D3D11TextureSampler::D3D11TextureSampler");

    D3D11_SAMPLER_DESC description = {};
    description.Filter = (maximumAnistropy > 1.0f) ? D3D11_FILTER_ANISOTROPIC : D3D11TypeConverter::ToFilter(minificationFilter, magnificationFilter);
    description.AddressU = D3D11TypeConverter::ToAddressMode(wrapS);
    description.AddressV = D3D11TypeConverter::ToAddressMode(wrapT);
    description.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    description.MaxAnisotropy = (UINT)maximumAnistropy;
    description.ComparisonFunc = D3D11_COMPARISON_NEVER;
    description.MinLOD = 0.0f;
    description.MaxLOD = D3D11_FLOAT32_MAX;

    const HRESULT result = device->CreateSamplerState(&description, &m_sampler);
    if (FAILED(result))
    {
        LOG_ERROR("CreateSamplerState failed (0x%08X).", (unsigned)result);
    }
}

D3D11TextureSampler::~D3D11TextureSampler() { FN("D3D11TextureSampler::~D3D11TextureSampler"); }

} // namespace bow
