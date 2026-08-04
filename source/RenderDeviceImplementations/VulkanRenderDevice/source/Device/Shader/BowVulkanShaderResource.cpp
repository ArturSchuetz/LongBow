#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResource.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

VulkanShaderResource::VulkanShaderResource(uint32_t _set, uint32_t _location, std::string _name, size_t _sizeInBytes, ShaderResourceType _type) : set(_set), binding(_location), name(_name), sizeInBytes(_sizeInBytes), resourceType(_type)
{
    FN("VulkanShaderResource::VulkanShaderResource");
}

VulkanPushConstant::VulkanPushConstant(std::string _name, size_t _sizeInBytes) : name(_name), sizeInBytes(_sizeInBytes) { FN("VulkanPushConstant::VulkanPushConstant"); }

} // namespace bow