#include "BowD3D12TextureSampler.h"
#include "BowLogger.h"

#include "BowD3D12TypeConverter.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	D3DTextureSampler::D3DTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
		: ITextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy)
	{
		m_sampler = {};
		m_sampler.Filter = D3DTypeConverter::To(minificationFilter, magnificationFilter);
		m_sampler.AddressU = D3DTypeConverter::To(wrapS);
		m_sampler.AddressV = D3DTypeConverter::To(wrapT);
		m_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		m_sampler.MipLODBias = FLOAT(0);
		m_sampler.MaxAnisotropy = UINT(maximumAnistropy);
		m_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		m_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		m_sampler.MinLOD = FLOAT(0.0f);
		m_sampler.MaxLOD = FLOAT(D3D12_FLOAT32_MAX);
		m_sampler.ShaderRegister = UINT(0);
		m_sampler.RegisterSpace = UINT(0);
		m_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
	}

	D3DTextureSampler::~D3DTextureSampler()
	{

	}

	D3D12_STATIC_SAMPLER_DESC D3DTextureSampler::GetDxSampler()
	{
		return m_sampler;
	}
}
