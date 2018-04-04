#include "BowD3D12TypeConverter.h"
#include "BowLogger.h"

#include "IBowRenderContext.h"

#include "BowIndexBufferDatatype.h"
#include "BowComponentDatatype.h"
#include "BowShaderVertexAttribute.h"

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

}
