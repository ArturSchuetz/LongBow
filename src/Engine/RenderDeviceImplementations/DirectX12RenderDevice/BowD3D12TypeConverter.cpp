#include "BowD3D12TypeConverter.h"
#include "BowLogger.h"

#include "IBowRenderContext.h"

// RenderState
#include "IBowRenderContext.h"
#include "BowClearState.h"
#include "BowRenderState.h"

// Shader
#include "BowShaderVertexAttribute.h"

// Buffer
#include "BowIndexBufferDatatype.h"
#include "BowComponentDatatype.h"

// Texture
#include "IBowTextureSampler.h"
#include "IBowTexture2D.h"

namespace bow {

	ShaderVertexAttributeType D3DTypeConverter::ToActiveAttribType(D3D_REGISTER_COMPONENT_TYPE type)
	{
		switch (type)
		{
		case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_FLOAT32:
			return ShaderVertexAttributeType::Float;
		case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_SINT32:
			return ShaderVertexAttributeType::Int;
		case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_UINT32:
			return ShaderVertexAttributeType::UnsignedInt;
		}

		LOG_FATAL("ActiveAttribType does not exist.");
		return ShaderVertexAttributeType::INVALID;
	}

	D3D_PRIMITIVE_TOPOLOGY D3DTypeConverter::To(PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Points:
			return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PrimitiveType::Lines:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimitiveType::LineStrip:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PrimitiveType::Triangles:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PrimitiveType::TriangleStrip:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case PrimitiveType::LinesAdjacency:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case PrimitiveType::LineStripAdjacency:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
		case PrimitiveType::TrianglesAdjacency:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		case PrimitiveType::TriangleStripAdjacency:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
		}

		LOG_FATAL("PrimitiveType does not exist.");
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}


	DXGI_FORMAT D3DTypeConverter::To(IndexBufferDatatype dataType)
	{
		switch (dataType)
		{
		case IndexBufferDatatype::UnsignedShort:
			return DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
		case IndexBufferDatatype::UnsignedInt:
			return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
		}
		LOG_FATAL("DXGI format does not exist.");
		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}


	DXGI_FORMAT D3DTypeConverter::To(int numOfComponents, ComponentDatatype dataType)
	{
		switch (dataType)
		{
		case ComponentDatatype::Byte:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R8_SINT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R8G8_SINT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		case ComponentDatatype::UnsignedByte:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R8_UINT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R8G8_UINT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		case ComponentDatatype::Short:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R16_SINT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R16G16_SINT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SINT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		case ComponentDatatype::UnsignedShort:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		case ComponentDatatype::Int:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R32_SINT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT;
			case 3:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		case ComponentDatatype::UnsignedInt:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
			case 3:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		case ComponentDatatype::Float:
			switch (numOfComponents)
			{
			case 1:
				return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			case 2:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
			case 3:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
			case 4:
				return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
			default:
				return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			}
		}

		LOG_FATAL("DXGI format does not exist.");
		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT D3DTypeConverter::To(TextureFormat textureFormat)
	{
		switch (textureFormat)
		{
		case TextureFormat::RedGreenBlue8:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::RedGreenBlue16:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::RedGreenBlueAlpha8:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::RedGreenBlue10A2:
			return DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM;
		case TextureFormat::RedGreenBlueAlpha16:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM;
		case TextureFormat::Depth16:
			return DXGI_FORMAT::DXGI_FORMAT_D16_UNORM;
		case TextureFormat::Depth24:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		case TextureFormat::Red8:
			return DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
		case TextureFormat::Red16:
			return DXGI_FORMAT::DXGI_FORMAT_R16_UNORM;
		case TextureFormat::RedGreen8:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM;
		case TextureFormat::RedGreen16:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_UNORM;
		case TextureFormat::Red16f:
			return DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		case TextureFormat::Red32f:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		case TextureFormat::RedGreen16f:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT;
		case TextureFormat::RedGreen32f:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		case TextureFormat::Red8i:
			return DXGI_FORMAT::DXGI_FORMAT_R8_SINT;
		case TextureFormat::Red8ui:
			return DXGI_FORMAT::DXGI_FORMAT_R8_UINT;
		case TextureFormat::Red16i:
			return DXGI_FORMAT::DXGI_FORMAT_R16_SINT;
		case TextureFormat::Red16ui:
			return DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
		case TextureFormat::Red32i:
			return DXGI_FORMAT::DXGI_FORMAT_R32_SINT;
		case TextureFormat::Red32ui:
			return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
		case TextureFormat::RedGreen8i:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8_SINT;
		case TextureFormat::RedGreen8ui:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8_UINT;
		case TextureFormat::RedGreen16i:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_SINT;
		case TextureFormat::RedGreen16ui:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT;
		case TextureFormat::RedGreen32i:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT;
		case TextureFormat::RedGreen32ui:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
		case TextureFormat::RedGreenBlueAlpha32f:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		case TextureFormat::RedGreenBlue32f:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		case TextureFormat::RedGreenBlueAlpha16f:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		case TextureFormat::RedGreenBlue16f:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::Depth24Stencil8:
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		case TextureFormat::Red11fGreen11fBlue10f:
			return DXGI_FORMAT::DXGI_FORMAT_R11G11B10_FLOAT;
		case TextureFormat::RedGreenBlue9E5:
			return DXGI_FORMAT::DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case TextureFormat::SRedGreenBlue8:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::SRedGreenBlue8Alpha8:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::Depth32f:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		case TextureFormat::Depth32fStencil8:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case TextureFormat::RedGreenBlueAlpha32ui:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
		case TextureFormat::RedGreenBlue32ui:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
		case TextureFormat::RedGreenBlueAlpha16ui:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT;
		case TextureFormat::RedGreenBlue16ui:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::RedGreenBlueAlpha8ui:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT;
		case TextureFormat::RedGreenBlue8ui:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::RedGreenBlueAlpha32i:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT;
		case TextureFormat::RedGreenBlue32i:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT;
		case TextureFormat::RedGreenBlueAlpha16i:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SINT;
		case TextureFormat::RedGreenBlue16i:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case TextureFormat::RedGreenBlueAlpha8i:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT;
		case TextureFormat::RedGreenBlue8i:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		}

		LOG_FATAL("Texture format does not exist.");
		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	ImageFormat D3DTypeConverter::TextureToImageFormat(TextureFormat textureFormat)
	{
		if (!IsTextureFormatValid(textureFormat))
		{
			LOG_FATAL("Invalid texture format::textureFormat");
		}

		// TODO:  Not tested exhaustively
		switch (textureFormat)
		{
		case TextureFormat::RedGreenBlue8:
		case TextureFormat::RedGreenBlue8i:
		case TextureFormat::RedGreenBlue8ui:
		case TextureFormat::RedGreenBlue16:
		case TextureFormat::RedGreenBlue16f:
		case TextureFormat::RedGreenBlue16i:
		case TextureFormat::RedGreenBlue16ui:
		case TextureFormat::RedGreenBlue32f:
		case TextureFormat::RedGreenBlue32i:
		case TextureFormat::RedGreenBlue32ui:
			return ImageFormat::RedGreenBlue;
		case TextureFormat::RedGreenBlueAlpha8:
		case TextureFormat::RedGreenBlueAlpha8i:
		case TextureFormat::RedGreenBlueAlpha8ui:
		case TextureFormat::RedGreenBlueAlpha16:
		case TextureFormat::RedGreenBlueAlpha16f:
		case TextureFormat::RedGreenBlueAlpha16i:
		case TextureFormat::RedGreenBlueAlpha16ui:
		case TextureFormat::RedGreenBlueAlpha32f:
		case TextureFormat::RedGreenBlueAlpha32i:
		case TextureFormat::RedGreenBlueAlpha32ui:
			return ImageFormat::RedGreenBlueAlpha;
		}
		return ImageFormat::RedGreenBlue;
	}

	bool D3DTypeConverter::IsTextureFormatValid(TextureFormat textureFormat)
	{
		return (textureFormat == TextureFormat::RedGreenBlue8) ||
			(textureFormat == TextureFormat::RedGreenBlue16) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha8) ||
			(textureFormat == TextureFormat::RedGreenBlue10A2) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha16) ||
			(textureFormat == TextureFormat::Depth16) ||
			(textureFormat == TextureFormat::Depth24) ||
			(textureFormat == TextureFormat::Red8) ||
			(textureFormat == TextureFormat::Red16) ||
			(textureFormat == TextureFormat::RedGreen8) ||
			(textureFormat == TextureFormat::RedGreen16) ||
			(textureFormat == TextureFormat::Red16f) ||
			(textureFormat == TextureFormat::Red32f) ||
			(textureFormat == TextureFormat::RedGreen16f) ||
			(textureFormat == TextureFormat::RedGreen32f) ||
			(textureFormat == TextureFormat::Red8i) ||
			(textureFormat == TextureFormat::Red8ui) ||
			(textureFormat == TextureFormat::Red16i) ||
			(textureFormat == TextureFormat::Red16ui) ||
			(textureFormat == TextureFormat::Red32i) ||
			(textureFormat == TextureFormat::Red32ui) ||
			(textureFormat == TextureFormat::RedGreen8i) ||
			(textureFormat == TextureFormat::RedGreen8ui) ||
			(textureFormat == TextureFormat::RedGreen16i) ||
			(textureFormat == TextureFormat::RedGreen16ui) ||
			(textureFormat == TextureFormat::RedGreen32i) ||
			(textureFormat == TextureFormat::RedGreen32ui) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha32f) ||
			(textureFormat == TextureFormat::RedGreenBlue32f) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha16f) ||
			(textureFormat == TextureFormat::RedGreenBlue16f) ||
			(textureFormat == TextureFormat::Depth24Stencil8) ||
			(textureFormat == TextureFormat::Red11fGreen11fBlue10f) ||
			(textureFormat == TextureFormat::RedGreenBlue9E5) ||
			(textureFormat == TextureFormat::SRedGreenBlue8) ||
			(textureFormat == TextureFormat::SRedGreenBlue8Alpha8) ||
			(textureFormat == TextureFormat::Depth32f) ||
			(textureFormat == TextureFormat::Depth32fStencil8) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha32ui) ||
			(textureFormat == TextureFormat::RedGreenBlue32ui) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha16ui) ||
			(textureFormat == TextureFormat::RedGreenBlue16ui) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha8ui) ||
			(textureFormat == TextureFormat::RedGreenBlue8ui) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha32i) ||
			(textureFormat == TextureFormat::RedGreenBlue32i) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha16i) ||
			(textureFormat == TextureFormat::RedGreenBlue16i) ||
			(textureFormat == TextureFormat::RedGreenBlueAlpha8i) ||
			(textureFormat == TextureFormat::RedGreenBlue8i);
	}

	D3D12_FILTER D3DTypeConverter::To(TextureMinificationFilter minfilter, TextureMagnificationFilter maxfilter)
	{
		if (minfilter == TextureMinificationFilter::Nearest && maxfilter == TextureMagnificationFilter::Nearest)
		{
			return D3D12_FILTER_MIN_MAG_MIP_POINT;
		}
		else if (minfilter == TextureMinificationFilter::Linear && maxfilter == TextureMagnificationFilter::Nearest)
		{
			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		}
		else if (minfilter == TextureMinificationFilter::Nearest && maxfilter == TextureMagnificationFilter::Linear)
		{
			return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
		else if (minfilter == TextureMinificationFilter::Linear && maxfilter == TextureMagnificationFilter::Linear)
		{
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}
		else if (minfilter == TextureMinificationFilter::NearestMipmapNearest && maxfilter == TextureMagnificationFilter::Nearest)
		{
			return D3D12_FILTER_MIN_MAG_MIP_POINT;
		}
		else if(minfilter == TextureMinificationFilter::NearestMipmapLinear && maxfilter == TextureMagnificationFilter::Nearest)
		{
			return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		}
		else if(minfilter == TextureMinificationFilter::NearestMipmapNearest && maxfilter == TextureMagnificationFilter::Linear)
		{
			return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
		else if (minfilter == TextureMinificationFilter::NearestMipmapLinear && maxfilter == TextureMagnificationFilter::Linear)
		{
			return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		}
		else if (minfilter == TextureMinificationFilter::LinearMipmapNearest && maxfilter == TextureMagnificationFilter::Nearest)
		{
			return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}
		else if (minfilter == TextureMinificationFilter::LinearMipmapLinear && maxfilter == TextureMagnificationFilter::Nearest)
		{
			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		}
		else if (minfilter == TextureMinificationFilter::LinearMipmapNearest && maxfilter == TextureMagnificationFilter::Linear)
		{
			return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
		else if (minfilter == TextureMinificationFilter::LinearMipmapLinear && maxfilter == TextureMagnificationFilter::Linear)
		{
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}
		else
		{
			LOG_FATAL("Min Mag filter combination does not exist.");
			return D3D12_FILTER_ANISOTROPIC;
		}
	}

	D3D12_TEXTURE_ADDRESS_MODE D3DTypeConverter::To(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Clamp:
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureWrap::MirroredRepeat:
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case TextureWrap::Repeat:
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}

		LOG_FATAL("Warp Mode does not exist.");
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	}
}
