#include <VulkanThinDevice/BowVulkanThinCommon.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

bool VulkanCheck(VkResult result, const char *what)
{
    if (result == VK_SUCCESS)
    {
        return true;
    }

    LOG_ERROR("%s failed: %s", what, VulkanThinTypes::ToString(result));
    return false;
}

VkFormat VulkanThinTypes::ToVkFormat(ThinFormat format)
{
    switch (format)
    {
    case ThinFormat::R8Unorm:
        return VK_FORMAT_R8_UNORM;
    case ThinFormat::RG8Unorm:
        return VK_FORMAT_R8G8_UNORM;
    case ThinFormat::RGBA8Unorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case ThinFormat::RGBA8Srgb:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case ThinFormat::BGRA8Unorm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case ThinFormat::R16Float:
        return VK_FORMAT_R16_SFLOAT;
    case ThinFormat::RG16Float:
        return VK_FORMAT_R16G16_SFLOAT;
    case ThinFormat::RGBA16Float:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case ThinFormat::R32Float:
        return VK_FORMAT_R32_SFLOAT;
    case ThinFormat::RG32Float:
        return VK_FORMAT_R32G32_SFLOAT;
    case ThinFormat::RGB32Float:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case ThinFormat::RGBA32Float:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ThinFormat::R32Uint:
        return VK_FORMAT_R32_UINT;
    case ThinFormat::R16Uint:
        return VK_FORMAT_R16_UINT;
    case ThinFormat::D32Float:
        return VK_FORMAT_D32_SFLOAT;
    case ThinFormat::D24UnormS8Uint:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

ThinFormat VulkanThinTypes::FromVkFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R8G8B8A8_UNORM:
        return ThinFormat::RGBA8Unorm;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return ThinFormat::RGBA8Srgb;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return ThinFormat::BGRA8Unorm;
    case VK_FORMAT_D32_SFLOAT:
        return ThinFormat::D32Float;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return ThinFormat::D24UnormS8Uint;
    default:
        return ThinFormat::Unknown;
    }
}

VkImageLayout VulkanThinTypes::ToImageLayout(ThinResourceState state)
{
    switch (state)
    {
    case ThinResourceState::Undefined:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    case ThinResourceState::ShaderResource:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case ThinResourceState::UnorderedAccess:
        return VK_IMAGE_LAYOUT_GENERAL;
    case ThinResourceState::RenderTarget:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case ThinResourceState::DepthWrite:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case ThinResourceState::DepthRead:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    case ThinResourceState::CopySource:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case ThinResourceState::CopyDestination:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case ThinResourceState::Present:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    default:
        return VK_IMAGE_LAYOUT_GENERAL;
    }
}

VkAccessFlags2 VulkanThinTypes::ToAccessFlags(ThinResourceState state)
{
    switch (state)
    {
    case ThinResourceState::Undefined:
        return VK_ACCESS_2_NONE;
    case ThinResourceState::VertexBuffer:
        return VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    case ThinResourceState::IndexBuffer:
        return VK_ACCESS_2_INDEX_READ_BIT;
    case ThinResourceState::ConstantBuffer:
        return VK_ACCESS_2_UNIFORM_READ_BIT;
    case ThinResourceState::IndirectArgument:
        return VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
    case ThinResourceState::ShaderResource:
        return VK_ACCESS_2_SHADER_READ_BIT;
    case ThinResourceState::UnorderedAccess:
        return VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
    case ThinResourceState::RenderTarget:
        return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    case ThinResourceState::DepthWrite:
        return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    case ThinResourceState::DepthRead:
        return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    case ThinResourceState::CopySource:
        return VK_ACCESS_2_TRANSFER_READ_BIT;
    case ThinResourceState::CopyDestination:
        return VK_ACCESS_2_TRANSFER_WRITE_BIT;
    case ThinResourceState::Present:
        return VK_ACCESS_2_NONE;
    case ThinResourceState::AccelerationStructure:
        return VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    default:
        return VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
    }
}

VkPipelineStageFlags2 VulkanThinTypes::ToStageFlags(ThinResourceState state)
{
    switch (state)
    {
    case ThinResourceState::Undefined:
        return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    case ThinResourceState::VertexBuffer:
    case ThinResourceState::IndexBuffer:
        return VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
    case ThinResourceState::IndirectArgument:
        return VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
    case ThinResourceState::ConstantBuffer:
    case ThinResourceState::ShaderResource:
    case ThinResourceState::UnorderedAccess:
        return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    case ThinResourceState::RenderTarget:
        return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    case ThinResourceState::DepthWrite:
    case ThinResourceState::DepthRead:
        return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
    case ThinResourceState::CopySource:
    case ThinResourceState::CopyDestination:
        return VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
    case ThinResourceState::Present:
        return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    case ThinResourceState::AccelerationStructure:
        return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
    default:
        return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    }
}

VkShaderStageFlagBits VulkanThinTypes::ToShaderStage(ThinShaderStage stage)
{
    switch (stage)
    {
    case ThinShaderStage::Vertex:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case ThinShaderStage::Hull:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case ThinShaderStage::Domain:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case ThinShaderStage::Geometry:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    case ThinShaderStage::Fragment:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ThinShaderStage::Compute:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    case ThinShaderStage::RayGeneration:
        return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    case ThinShaderStage::AnyHit:
        return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    case ThinShaderStage::ClosestHit:
        return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    case ThinShaderStage::Miss:
        return VK_SHADER_STAGE_MISS_BIT_KHR;
    case ThinShaderStage::Intersection:
        return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    case ThinShaderStage::Callable:
        return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    default:
        return VK_SHADER_STAGE_VERTEX_BIT;
    }
}

VkShaderStageFlags VulkanThinTypes::ToShaderStageFlags(ThinShaderStage stages)
{
    VkShaderStageFlags flags = 0;
    const ThinShaderStage all[] = {ThinShaderStage::Vertex,        ThinShaderStage::Hull,   ThinShaderStage::Domain, ThinShaderStage::Geometry, ThinShaderStage::Fragment,     ThinShaderStage::Compute,
                                   ThinShaderStage::RayGeneration, ThinShaderStage::AnyHit, ThinShaderStage::ClosestHit, ThinShaderStage::Miss,  ThinShaderStage::Intersection, ThinShaderStage::Callable};

    for (ThinShaderStage stage : all)
    {
        if (stages & stage)
        {
            flags |= ToShaderStage(stage);
        }
    }
    return flags;
}

VkDescriptorType VulkanThinTypes::ToDescriptorType(ThinDescriptorType type)
{
    switch (type)
    {
    case ThinDescriptorType::ConstantBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case ThinDescriptorType::StorageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case ThinDescriptorType::SampledTexture:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case ThinDescriptorType::StorageTexture:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case ThinDescriptorType::Sampler:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case ThinDescriptorType::AccelerationStructure:
        return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    default:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

VkPrimitiveTopology VulkanThinTypes::ToPrimitiveTopology(ThinPrimitiveTopology topology)
{
    switch (topology)
    {
    case ThinPrimitiveTopology::PointList:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case ThinPrimitiveTopology::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case ThinPrimitiveTopology::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case ThinPrimitiveTopology::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    default:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkCompareOp VulkanThinTypes::ToCompareOp(ThinCompareOp op)
{
    switch (op)
    {
    case ThinCompareOp::Never:
        return VK_COMPARE_OP_NEVER;
    case ThinCompareOp::Less:
        return VK_COMPARE_OP_LESS;
    case ThinCompareOp::Equal:
        return VK_COMPARE_OP_EQUAL;
    case ThinCompareOp::LessOrEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case ThinCompareOp::Greater:
        return VK_COMPARE_OP_GREATER;
    case ThinCompareOp::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case ThinCompareOp::GreaterOrEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    default:
        return VK_COMPARE_OP_ALWAYS;
    }
}

VkCullModeFlags VulkanThinTypes::ToCullMode(ThinCullMode mode)
{
    switch (mode)
    {
    case ThinCullMode::Front:
        return VK_CULL_MODE_FRONT_BIT;
    case ThinCullMode::Back:
        return VK_CULL_MODE_BACK_BIT;
    default:
        return VK_CULL_MODE_NONE;
    }
}

VkBlendFactor VulkanThinTypes::ToBlendFactor(ThinBlendFactor factor)
{
    switch (factor)
    {
    case ThinBlendFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case ThinBlendFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case ThinBlendFactor::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case ThinBlendFactor::OneMinusSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case ThinBlendFactor::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case ThinBlendFactor::OneMinusDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    default:
        return VK_BLEND_FACTOR_ONE;
    }
}

VkAttachmentLoadOp VulkanThinTypes::ToLoadOp(ThinLoadOp op)
{
    switch (op)
    {
    case ThinLoadOp::Load:
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    case ThinLoadOp::Clear:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    default:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

VkAttachmentStoreOp VulkanThinTypes::ToStoreOp(ThinStoreOp op) { return (op == ThinStoreOp::Store) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE; }

const char *VulkanThinTypes::ToString(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS:
        return "VK_SUCCESS";
    case VK_NOT_READY:
        return "VK_NOT_READY";
    case VK_TIMEOUT:
        return "VK_TIMEOUT";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR";
    default:
        return "unknown VkResult";
    }
}

} // namespace bow
