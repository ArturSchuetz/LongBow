#include <DirectX12ThinDevice/BowD3D12ThinCommon.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

bool D3D12Check(HRESULT result, const char *what)
{
    if (SUCCEEDED(result))
    {
        return true;
    }

    LOG_ERROR("%s failed: %s (0x%08X)", what, D3D12ThinTypes::ToString(result), (unsigned)result);
    return false;
}

DXGI_FORMAT D3D12ThinTypes::ToDXGIFormat(ThinFormat format)
{
    switch (format)
    {
    case ThinFormat::R8Unorm:
        return DXGI_FORMAT_R8_UNORM;
    case ThinFormat::RG8Unorm:
        return DXGI_FORMAT_R8G8_UNORM;
    case ThinFormat::RGBA8Unorm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case ThinFormat::RGBA8Srgb:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case ThinFormat::BGRA8Unorm:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case ThinFormat::R16Float:
        return DXGI_FORMAT_R16_FLOAT;
    case ThinFormat::RG16Float:
        return DXGI_FORMAT_R16G16_FLOAT;
    case ThinFormat::RGBA16Float:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case ThinFormat::R32Float:
        return DXGI_FORMAT_R32_FLOAT;
    case ThinFormat::RG32Float:
        return DXGI_FORMAT_R32G32_FLOAT;
    case ThinFormat::RGB32Float:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case ThinFormat::RGBA32Float:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case ThinFormat::R32Uint:
        return DXGI_FORMAT_R32_UINT;
    case ThinFormat::R16Uint:
        return DXGI_FORMAT_R16_UINT;
    case ThinFormat::D32Float:
        return DXGI_FORMAT_D32_FLOAT;
    case ThinFormat::D24UnormS8Uint:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

ThinFormat D3D12ThinTypes::FromDXGIFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return ThinFormat::RGBA8Unorm;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return ThinFormat::RGBA8Srgb;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return ThinFormat::BGRA8Unorm;
    case DXGI_FORMAT_D32_FLOAT:
        return ThinFormat::D32Float;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return ThinFormat::D24UnormS8Uint;
    default:
        return ThinFormat::Unknown;
    }
}

D3D12_RESOURCE_STATES D3D12ThinTypes::ToResourceStates(ThinResourceState state)
{
    switch (state)
    {
    case ThinResourceState::Undefined:
        return D3D12_RESOURCE_STATE_COMMON;
    case ThinResourceState::General:
        return D3D12_RESOURCE_STATE_COMMON;
    case ThinResourceState::VertexBuffer:
    case ThinResourceState::ConstantBuffer:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    case ThinResourceState::IndexBuffer:
        return D3D12_RESOURCE_STATE_INDEX_BUFFER;
    case ThinResourceState::IndirectArgument:
        return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    case ThinResourceState::ShaderResource:
        return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    case ThinResourceState::UnorderedAccess:
        return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    case ThinResourceState::RenderTarget:
        return D3D12_RESOURCE_STATE_RENDER_TARGET;
    case ThinResourceState::DepthWrite:
        return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    case ThinResourceState::DepthRead:
        return D3D12_RESOURCE_STATE_DEPTH_READ;
    case ThinResourceState::CopySource:
        return D3D12_RESOURCE_STATE_COPY_SOURCE;
    case ThinResourceState::CopyDestination:
        return D3D12_RESOURCE_STATE_COPY_DEST;
    case ThinResourceState::Present:
        return D3D12_RESOURCE_STATE_PRESENT;
    case ThinResourceState::AccelerationStructure:
        return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    default:
        return D3D12_RESOURCE_STATE_COMMON;
    }
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12ThinTypes::ToTopologyType(ThinPrimitiveTopology topology)
{
    switch (topology)
    {
    case ThinPrimitiveTopology::PointList:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case ThinPrimitiveTopology::LineList:
    case ThinPrimitiveTopology::LineStrip:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    default:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }
}

D3D12_PRIMITIVE_TOPOLOGY D3D12ThinTypes::ToTopology(ThinPrimitiveTopology topology)
{
    switch (topology)
    {
    case ThinPrimitiveTopology::PointList:
        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case ThinPrimitiveTopology::LineList:
        return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case ThinPrimitiveTopology::LineStrip:
        return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case ThinPrimitiveTopology::TriangleStrip:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    default:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}

D3D12_COMPARISON_FUNC D3D12ThinTypes::ToComparisonFunc(ThinCompareOp op)
{
    switch (op)
    {
    case ThinCompareOp::Never:
        return D3D12_COMPARISON_FUNC_NEVER;
    case ThinCompareOp::Less:
        return D3D12_COMPARISON_FUNC_LESS;
    case ThinCompareOp::Equal:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case ThinCompareOp::LessOrEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case ThinCompareOp::Greater:
        return D3D12_COMPARISON_FUNC_GREATER;
    case ThinCompareOp::NotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case ThinCompareOp::GreaterOrEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    default:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    }
}

D3D12_CULL_MODE D3D12ThinTypes::ToCullMode(ThinCullMode mode)
{
    switch (mode)
    {
    case ThinCullMode::Front:
        return D3D12_CULL_MODE_FRONT;
    case ThinCullMode::Back:
        return D3D12_CULL_MODE_BACK;
    default:
        return D3D12_CULL_MODE_NONE;
    }
}

D3D12_BLEND D3D12ThinTypes::ToBlend(ThinBlendFactor factor)
{
    switch (factor)
    {
    case ThinBlendFactor::Zero:
        return D3D12_BLEND_ZERO;
    case ThinBlendFactor::One:
        return D3D12_BLEND_ONE;
    case ThinBlendFactor::SourceAlpha:
        return D3D12_BLEND_SRC_ALPHA;
    case ThinBlendFactor::OneMinusSourceAlpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case ThinBlendFactor::DestinationAlpha:
        return D3D12_BLEND_DEST_ALPHA;
    case ThinBlendFactor::OneMinusDestinationAlpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
    default:
        return D3D12_BLEND_ONE;
    }
}

D3D12_SHADER_VISIBILITY D3D12ThinTypes::ToShaderVisibility(ThinShaderStage stages)
{
    // DirectX names one stage or all of them, where Vulkan takes a mask. A
    // combination that is not exactly one stage therefore becomes ALL.
    if (stages == ThinShaderStage::Vertex)
        return D3D12_SHADER_VISIBILITY_VERTEX;
    if (stages == ThinShaderStage::Fragment)
        return D3D12_SHADER_VISIBILITY_PIXEL;
    if (stages == ThinShaderStage::Geometry)
        return D3D12_SHADER_VISIBILITY_GEOMETRY;
    if (stages == ThinShaderStage::Hull)
        return D3D12_SHADER_VISIBILITY_HULL;
    if (stages == ThinShaderStage::Domain)
        return D3D12_SHADER_VISIBILITY_DOMAIN;

    return D3D12_SHADER_VISIBILITY_ALL;
}

D3D12_DESCRIPTOR_RANGE_TYPE D3D12ThinTypes::ToDescriptorRangeType(ThinDescriptorType type)
{
    switch (type)
    {
    case ThinDescriptorType::ConstantBuffer:
        return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    case ThinDescriptorType::StorageBuffer:
    case ThinDescriptorType::StorageTexture:
        return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    case ThinDescriptorType::Sampler:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    default:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    }
}

const char *D3D12ThinTypes::ToString(HRESULT result)
{
    switch (result)
    {
    case S_OK:
        return "S_OK";
    case E_OUTOFMEMORY:
        return "E_OUTOFMEMORY";
    case E_INVALIDARG:
        return "E_INVALIDARG";
    case E_NOTIMPL:
        return "E_NOTIMPL";
    case DXGI_ERROR_DEVICE_REMOVED:
        return "DXGI_ERROR_DEVICE_REMOVED";
    case DXGI_ERROR_DEVICE_RESET:
        return "DXGI_ERROR_DEVICE_RESET";
    case DXGI_ERROR_INVALID_CALL:
        return "DXGI_ERROR_INVALID_CALL";
    case DXGI_ERROR_UNSUPPORTED:
        return "DXGI_ERROR_UNSUPPORTED";
    default:
        return "unknown HRESULT";
    }
}

} // namespace bow
