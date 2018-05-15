#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	class D3DTypeConverter
	{
	public:
		static ShaderVertexAttributeType	ToActiveAttribType(D3D_REGISTER_COMPONENT_TYPE type);
		static D3D_PRIMITIVE_TOPOLOGY		To(PrimitiveType type);

		// Index and Vertex Buffer
		static DXGI_FORMAT					To(IndexBufferDatatype dataType);
		static DXGI_FORMAT					To(int numOfComponents, ComponentDatatype dataType);

		// Texture
		static DXGI_FORMAT					To(TextureFormat textureFormat);
		static ImageFormat					TextureToImageFormat(TextureFormat textureFormat);
		static bool							IsTextureFormatValid(TextureFormat textureFormat);
		static D3D12_FILTER					To(TextureMinificationFilter minfilter, TextureMagnificationFilter maxfilter);
		static D3D12_TEXTURE_ADDRESS_MODE	To(TextureWrap wrap);
	};

}
