#include <VulkanRenderDevice/Device/RayTracing/BowVulkanRayTracingShaderProgram.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResource.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResourceBindings.h>

#include <RenderDevice/Device/IBowRenderContext.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

#include <shaderc/shaderc.hpp>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

std::vector<uint32_t> CompileGLSLToRayTracingSPIRV(const std::string &sourceCode, shaderc_shader_kind kind)
{
    FN("CompileGLSLToSPIRV");

    switch (kind)
    {
    case shaderc_raygen_shader:
        LOG_TRACE("Compiling ray gen shader");
        break;
    case shaderc_anyhit_shader:
        LOG_TRACE("Compiling any hit shader");
        break;
    case shaderc_closesthit_shader:
        LOG_TRACE("Compiling closest shader");
        break;
    case shaderc_miss_shader:
        LOG_TRACE("Compiling miss shader");
        break;
    case shaderc_intersection_shader:
        LOG_TRACE("Compiling intersection shader");
        break;
    case shaderc_callable_shader:
        LOG_TRACE("Compiling callable shader");
        break;
    default:
        LOG_ERROR("Unknown shader kind");
        return {};
    }

    auto start = std::chrono::high_resolution_clock::now();

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetGenerateDebugInfo();
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    options.SetTargetSpirv(shaderc_spirv_version_1_4);

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(sourceCode, kind, "shader.glsl", options);
    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        LOG_ERROR("Shader compilation failed: %s", module.GetErrorMessage().c_str());
        return {};
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    LOG_TRACE("Shader compilation successful, took %f seconds", duration.count());

    return {module.cbegin(), module.cend()};
}

static uint32_t alignUp(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

namespace bow
{

VulkanRayTracingShaderProgram::VulkanRayTracingShaderProgram(VulkanLogicalDevice *logicalDevice)
    : m_logicalDevice(logicalDevice), m_raygenShaderModule(VK_NULL_HANDLE), m_anyHitShaderModule(VK_NULL_HANDLE), m_closestHitShaderModule(VK_NULL_HANDLE), m_missShaderModule(VK_NULL_HANDLE), m_intersectionShaderModule(VK_NULL_HANDLE),
      m_callableShaderModule(VK_NULL_HANDLE), m_descriptorPool(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE), m_pipeline(VK_NULL_HANDLE), m_sbtBuffer(nullptr), m_raygenSBTRegion{}, m_missSBTRegion{}, m_hitSBTRegion{}, m_callableSBTRegion{},
      m_guid(Utils::GenerateGUID())
{
    FN("VulkanRayTracingShaderProgram::VulkanRayTracingShaderProgram");
}

VulkanRayTracingShaderProgram::~VulkanRayTracingShaderProgram()
{
    FN("VulkanRayTracingShaderProgram::~VulkanRayTracingShaderProgram");
    Release();
}

bool VulkanRayTracingShaderProgram::Initialize(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource, const std::string &intersectionShaderSource,
                                               const std::string &callableShaderSource)
{
    FN("VulkanRayTracingShaderProgram::Initialize");

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    auto createShaderModule = [&](const std::string &shaderSource, ShaderStage shaderStage, shaderc_shader_kind kind, VkShaderModule &shaderModule, const char *shaderTypeName)
    {
        if (shaderSource.empty())
            return true;

        std::vector<uint32_t> shaderCompiled = CompileGLSLToRayTracingSPIRV(shaderSource, kind);
        if (shaderCompiled.empty())
        {
            LOG_ERROR("VulkanShaderProgram: Failed to compile %s shader", shaderTypeName);
            return false;
        }

        shaderModuleCreateInfo.codeSize = shaderCompiled.size() * sizeof(uint32_t);
        shaderModuleCreateInfo.pCode = shaderCompiled.data();

        VkResult result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }

        // Note: SPIRV-Cross reflection is skipped for RT shaders because the
        // vendored SPIRV-Cross version does not support OpTypeAccelerationStructureKHR.
        // Descriptor layouts for RT shaders are built manually in BuildDescriptorSetLayouts().

        return true;
    };

    if (!createShaderModule(rayGenShaderSource, ShaderStage::RayGen, shaderc_raygen_shader, m_raygenShaderModule, "ray generation"))
        return false;
    if (!createShaderModule(anyHitShaderSource, ShaderStage::AnyHit, shaderc_anyhit_shader, m_anyHitShaderModule, "any hit"))
        return false;
    if (!createShaderModule(closestHitShaderSource, ShaderStage::ClosestHit, shaderc_closesthit_shader, m_closestHitShaderModule, "closest hit"))
        return false;
    if (!createShaderModule(missShaderSource, ShaderStage::Miss, shaderc_miss_shader, m_missShaderModule, "miss"))
        return false;
    if (!createShaderModule(intersectionShaderSource, ShaderStage::Intersection, shaderc_intersection_shader, m_intersectionShaderModule, "intersection"))
        return false;
    if (!createShaderModule(callableShaderSource, ShaderStage::Callable, shaderc_callable_shader, m_callableShaderModule, "callable"))
        return false;

    // Build shader stages and shader groups
    uint32_t stageIndex = 0;

    auto addShaderStage = [&](VkShaderModule shaderModule, VkShaderStageFlagBits stage) -> uint32_t
    {
        if (shaderModule == VK_NULL_HANDLE)
            return VK_SHADER_UNUSED_KHR;

        VkPipelineShaderStageCreateInfo shaderStageInfo = {};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = stage;
        shaderStageInfo.module = shaderModule;
        shaderStageInfo.pName = "main";
        m_shaderStages.push_back(shaderStageInfo);
        return stageIndex++;
    };

    uint32_t raygenIdx = addShaderStage(m_raygenShaderModule, VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    uint32_t missIdx = addShaderStage(m_missShaderModule, VK_SHADER_STAGE_MISS_BIT_KHR);
    uint32_t closestHitIdx = addShaderStage(m_closestHitShaderModule, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    uint32_t anyHitIdx = addShaderStage(m_anyHitShaderModule, VK_SHADER_STAGE_ANY_HIT_BIT_KHR);

    // Raygen group
    if (raygenIdx != VK_SHADER_UNUSED_KHR)
    {
        VkRayTracingShaderGroupCreateInfoKHR group = {};
        group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        group.generalShader = raygenIdx;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        m_shaderGroups.push_back(group);
    }

    // Miss group
    if (missIdx != VK_SHADER_UNUSED_KHR)
    {
        VkRayTracingShaderGroupCreateInfoKHR group = {};
        group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        group.generalShader = missIdx;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        m_shaderGroups.push_back(group);
    }

    // Hit group (triangles)
    if (closestHitIdx != VK_SHADER_UNUSED_KHR || anyHitIdx != VK_SHADER_UNUSED_KHR)
    {
        VkRayTracingShaderGroupCreateInfoKHR group = {};
        group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        group.closestHitShader = closestHitIdx;
        group.anyHitShader = anyHitIdx;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        m_shaderGroups.push_back(group);
    }

    // Descriptor layouts, pool, pipeline and SBT are created in Build(),
    // which must be called after AddDescriptorBinding() calls.

    return true;
}

bool VulkanRayTracingShaderProgram::VBuild()
{
    FN("VulkanRayTracingShaderProgram::VBuild");

    if (!BuildDescriptorSetLayouts(m_descriptorBindings))
    {
        return false;
    }

    // Create descriptor pool from registered bindings
    std::vector<VkDescriptorPoolSize> poolSizes;
    auto addPoolSize = [&](VkDescriptorType type, uint32_t count)
    {
        for (auto &ps : poolSizes)
        {
            if (ps.type == type)
            {
                ps.descriptorCount += count;
                return;
            }
        }
        poolSizes.push_back({type, count});
    };

    for (auto &b : m_descriptorBindings)
    {
        addPoolSize(b.descriptorType, 16);
    }

    if (poolSizes.empty())
    {
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1});
    }

    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCreateInfo.maxSets = 16;
    poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolCreateInfo.pPoolSizes = poolSizes.data();

    VkResult result = vkCreateDescriptorPool(m_logicalDevice->GetHandle(), &poolCreateInfo, nullptr, &m_descriptorPool);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create descriptor pool: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    if (!CreatePipelineAndSBT())
    {
        return false;
    }

    return true;
}

bool VulkanRayTracingShaderProgram::CreatePipelineAndSBT()
{
    FN("VulkanRayTracingShaderProgram::CreatePipelineAndSBT");

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();

    VkResult result = vkCreatePipelineLayout(m_logicalDevice->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create pipeline layout: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    // Create ray tracing pipeline
    VkRayTracingPipelineCreateInfoKHR pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipelineCreateInfo.pStages = m_shaderStages.data();
    pipelineCreateInfo.groupCount = static_cast<uint32_t>(m_shaderGroups.size());
    pipelineCreateInfo.pGroups = m_shaderGroups.data();
    pipelineCreateInfo.maxPipelineRayRecursionDepth = 1;
    pipelineCreateInfo.layout = m_pipelineLayout;

    result = vkCreateRayTracingPipelinesKHR(m_logicalDevice->GetHandle(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipeline);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create ray tracing pipeline: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    LOG_TRACE("Ray tracing pipeline created successfully");

    // Build Shader Binding Table
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties = m_logicalDevice->GetParentPhysicalDevice()->GetRayTracingPipelineProperties();

    uint32_t handleSize = rtProperties.shaderGroupHandleSize;
    uint32_t handleAlignment = rtProperties.shaderGroupHandleAlignment;
    uint32_t baseAlignment = rtProperties.shaderGroupBaseAlignment;
    uint32_t handleSizeAligned = alignUp(handleSize, handleAlignment);

    uint32_t groupCount = static_cast<uint32_t>(m_shaderGroups.size());

    // Get shader group handles
    uint32_t sbtSize = groupCount * handleSize;
    std::vector<uint8_t> shaderHandleStorage(sbtSize);
    result = vkGetRayTracingShaderGroupHandlesKHR(m_logicalDevice->GetHandle(), m_pipeline, 0, groupCount, sbtSize, shaderHandleStorage.data());
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to get shader group handles: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    // Layout: [raygen] [miss] [hit] [callable]
    // Each group region is base-aligned, each entry within is handle-aligned
    uint32_t raygenGroupCount = (m_raygenShaderModule != VK_NULL_HANDLE) ? 1 : 0;
    uint32_t missGroupCount = (m_missShaderModule != VK_NULL_HANDLE) ? 1 : 0;
    uint32_t hitGroupCount = (m_closestHitShaderModule != VK_NULL_HANDLE || m_anyHitShaderModule != VK_NULL_HANDLE) ? 1 : 0;
    uint32_t callableGroupCount = 0; // Not used yet

    uint32_t raygenRegionSize = alignUp(raygenGroupCount * handleSizeAligned, baseAlignment);
    uint32_t missRegionSize = alignUp(missGroupCount * handleSizeAligned, baseAlignment);
    uint32_t hitRegionSize = alignUp(hitGroupCount * handleSizeAligned, baseAlignment);
    uint32_t callableRegionSize = 0;

    uint32_t totalSBTSize = raygenRegionSize + missRegionSize + hitRegionSize + callableRegionSize;

    m_sbtBuffer = m_logicalDevice->CreateBuffer(
        VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, totalSBTSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Map and copy handles
    void *mapped = nullptr;
    vkMapMemory(m_logicalDevice->GetHandle(), m_sbtBuffer->GetDeviceMemory()->GetHandle(), 0, totalSBTSize, 0, &mapped);

    uint8_t *pData = static_cast<uint8_t *>(mapped);
    uint32_t handleIndex = 0;

    // Raygen
    if (raygenGroupCount > 0)
    {
        memcpy(pData, shaderHandleStorage.data() + handleIndex * handleSize, handleSize);
        handleIndex++;
    }

    // Miss
    if (missGroupCount > 0)
    {
        memcpy(pData + raygenRegionSize, shaderHandleStorage.data() + handleIndex * handleSize, handleSize);
        handleIndex++;
    }

    // Hit
    if (hitGroupCount > 0)
    {
        memcpy(pData + raygenRegionSize + missRegionSize, shaderHandleStorage.data() + handleIndex * handleSize, handleSize);
        handleIndex++;
    }

    vkUnmapMemory(m_logicalDevice->GetHandle(), m_sbtBuffer->GetDeviceMemory()->GetHandle());

    VkDeviceAddress sbtAddress = m_sbtBuffer->GetBufferDeviceAddress();

    m_raygenSBTRegion = {};
    m_raygenSBTRegion.deviceAddress = sbtAddress;
    m_raygenSBTRegion.stride = handleSizeAligned;
    m_raygenSBTRegion.size = raygenRegionSize;

    m_missSBTRegion = {};
    m_missSBTRegion.deviceAddress = sbtAddress + raygenRegionSize;
    m_missSBTRegion.stride = handleSizeAligned;
    m_missSBTRegion.size = missRegionSize;

    m_hitSBTRegion = {};
    m_hitSBTRegion.deviceAddress = sbtAddress + raygenRegionSize + missRegionSize;
    m_hitSBTRegion.stride = handleSizeAligned;
    m_hitSBTRegion.size = hitRegionSize;

    m_callableSBTRegion = {};

    LOG_TRACE("SBT created: raygen=%llu, miss=%llu, hit=%llu", m_raygenSBTRegion.deviceAddress, m_missSBTRegion.deviceAddress, m_hitSBTRegion.deviceAddress);

    return true;
}

void VulkanRayTracingShaderProgram::Release()
{
    FN("VulkanRayTracingShaderProgram::Release");

    if (m_logicalDevice == nullptr)
        return;

    VkDevice device = m_logicalDevice->GetHandle();

    if (m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    if (m_descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }

    for (auto &layout : m_descriptorSetLayouts)
    {
        vkDestroyDescriptorSetLayout(device, layout, nullptr);
    }
    m_descriptorSetLayouts.clear();

    m_shaderStages.clear();
    m_shaderGroups.clear();

    auto destroyModule = [&](VkShaderModule &mod)
    {
        if (mod != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, mod, nullptr);
            mod = VK_NULL_HANDLE;
        }
    };

    destroyModule(m_raygenShaderModule);
    destroyModule(m_anyHitShaderModule);
    destroyModule(m_closestHitShaderModule);
    destroyModule(m_missShaderModule);
    destroyModule(m_intersectionShaderModule);
    destroyModule(m_callableShaderModule);

    m_sbtBuffer = nullptr;
    m_logicalDevice = nullptr;
}

static VkDescriptorType ToVkDescriptorType(RTDescriptorType type)
{
    switch (type)
    {
    case RTDescriptorType::AccelerationStructure:
        return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    case RTDescriptorType::StorageImage:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case RTDescriptorType::UniformBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case RTDescriptorType::StorageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case RTDescriptorType::CombinedImageSampler:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

void VulkanRayTracingShaderProgram::VAddDescriptorBinding(uint32_t set, uint32_t binding, const std::string &name, RTDescriptorType type)
{
    VkDescriptorType vkType = ToVkDescriptorType(type);
    m_descriptorBindings.push_back({set, binding, name, vkType, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR});
}

ShaderResourceBindingsPtr VulkanRayTracingShaderProgram::VCreateResourceBindingObjects()
{
    FN("VulkanRayTracingShaderProgram::VCreateResourceBindingObjects");

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
    descriptorSetAllocateInfo.pSetLayouts = m_descriptorSetLayouts.data();

    std::vector<VkDescriptorSet> shaderDescriptorSets(m_descriptorSetLayouts.size());

    VkResult result = vkAllocateDescriptorSets(m_logicalDevice->GetHandle(), &descriptorSetAllocateInfo, shaderDescriptorSets.data());
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return nullptr;
    }

    VulkanShaderResourceBindingsPtr shaderBindings = std::make_shared<VulkanShaderResourceBindings>(m_logicalDevice, shaderDescriptorSets);

    // Build shader resource maps from manually registered descriptor bindings
    for (auto &b : m_descriptorBindings)
    {
        ShaderResourceType resType;
        switch (b.descriptorType)
        {
        case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            resType = ShaderResourceType::AccelerationStructure;
            break;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            resType = ShaderResourceType::StorageImage;
            break;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            resType = ShaderResourceType::UniformBuffer;
            break;
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            resType = ShaderResourceType::StorageBuffer;
            break;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            resType = ShaderResourceType::CombinedImageSampler;
            break;
        default:
            resType = ShaderResourceType::UniformBuffer;
            break;
        }
        m_shaderResources[ShaderStage::RayGen][b.binding] = std::make_shared<VulkanShaderResource>(b.set, b.binding, b.name, 0, resType);
    }

    for (auto it = m_shaderResources.begin(); it != m_shaderResources.end(); it++)
    {
        shaderBindings->SetShaderResources(it->first, it->second);
    }

    return shaderBindings;
}

// =================================================================================================
// PRIVATE METHODS
// =================================================================================================

std::vector<VkPipelineShaderStageCreateInfo> VulkanRayTracingShaderProgram::GetShaderStages() const
{
    FN("VulkanRayTracingShaderProgram::GetShaderStages");

    return m_shaderStages;
}

bool VulkanRayTracingShaderProgram::BuildDescriptorSetLayouts(const std::vector<DescriptorBindingInfo> &bindings)
{
    FN("VulkanRayTracingShaderProgram::BuildDescriptorSetLayouts");

    std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> layoutBindings;

    for (auto &b : bindings)
    {
        VkDescriptorSetLayoutBinding vkBinding = {};
        vkBinding.binding = b.binding;
        vkBinding.descriptorType = b.descriptorType;
        vkBinding.descriptorCount = 1;
        vkBinding.stageFlags = b.stageFlags;
        vkBinding.pImmutableSamplers = nullptr;
        layoutBindings[b.set].push_back(vkBinding);
    }

    uint32_t maxSetIndex = 0;
    for (auto &kv : layoutBindings)
    {
        if (kv.first >= maxSetIndex)
            maxSetIndex = kv.first + 1;
    }

    m_descriptorSetLayouts.resize(maxSetIndex);
    for (uint32_t i = 0; i < maxSetIndex; i++)
    {
        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        if (layoutBindings.count(i) > 0)
        {
            createInfo.bindingCount = static_cast<uint32_t>(layoutBindings[i].size());
            createInfo.pBindings = layoutBindings[i].data();
        }

        VkResult result = vkCreateDescriptorSetLayout(m_logicalDevice->GetHandle(), &createInfo, nullptr, &m_descriptorSetLayouts[i]);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create descriptor set layout: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }
    }

    return true;
}

} // namespace bow
