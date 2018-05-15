#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowTextureSampler.h"

// DirectX 12 specific headers.
#include <d3d12.h>

namespace bow {

	class D3DTextureSampler : public ITextureSampler
	{
	public:
		D3DTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1);
		~D3DTextureSampler();

		D3D12_STATIC_SAMPLER_DESC GetDxSampler();

	private:
		D3D12_STATIC_SAMPLER_DESC m_sampler;
	};

}
