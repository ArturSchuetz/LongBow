#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

enum class ShaderResourceType : uint8_t
{
    Sampler,
    CombinedImageSampler,
    SampledImage,
    SeparateImage,
    StorageImage,

    UniformBuffer,
    StorageBuffer,

    SubpassInput,
    AtomicCounter,
    AccelerationStructure,
};

struct VulkanShaderResource
{
    VulkanShaderResource(uint32_t set, uint32_t binding, std::string name, size_t sizeInBytes, ShaderResourceType type);

    const uint32_t set;
    const uint32_t binding;
    const std::string name;
    const size_t sizeInBytes;
    const ShaderResourceType resourceType;
};

struct VulkanPushConstant
{
    VulkanPushConstant(std::string name, size_t sizeInBytes);

    const std::string name;
    const size_t sizeInBytes;
};

} // namespace bow