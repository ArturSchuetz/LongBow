#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderProgram.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/BowVulkanPipeline.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanFragmentOutputs.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResource.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResourceBindings.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanRenderPass.h>

#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexAttributeBindingsSizes.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#include <optick.h>

#include <shaderc/shaderc.hpp>

#include <spirv_cross/spirv_cross.hpp>

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

std::vector<uint32_t> CompileGLSLToSPIRV(const std::string &sourceCode, shaderc_shader_kind kind)
{
    FN("CompileGLSLToSPIRV");

    switch (kind)
    {
    case shaderc_vertex_shader:
        LOG_TRACE("Compiling vertex shader");
        break;
    case shaderc_fragment_shader:
        LOG_TRACE("Compiling fragment shader");
        break;
    case shaderc_geometry_shader:
        LOG_TRACE("Compiling geometry shader");
        break;
    case shaderc_tess_control_shader:
        LOG_TRACE("Compiling tessellation control shader");
        break;
    case shaderc_tess_evaluation_shader:
        LOG_TRACE("Compiling tessellation evaluation shader");
        break;
    case shaderc_compute_shader:
        LOG_TRACE("Compiling compute shader");
        break;
    default:
        LOG_ERROR("Unknown shader kind");
        return {};
    }

    auto start = std::chrono::high_resolution_clock::now(); // Startzeitpunkt

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetGenerateDebugInfo(); // Generate debug info

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(sourceCode, kind, "shader.glsl", options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        LOG_ERROR("Shader compilation failed: %s", module.GetErrorMessage().c_str());
        return {};
    }

    auto end = std::chrono::high_resolution_clock::now(); // Endzeitpunkt
    std::chrono::duration<double> duration = end - start; // Dauer berechnen

    LOG_TRACE("Shader compilation successful, took %f seconds", duration.count());

    return {module.cbegin(), module.cend()};
}

namespace bow
{

VulkanShaderProgram::VulkanShaderProgram()
    : m_logicalDevice(nullptr), m_vertexShaderModule(VK_NULL_HANDLE), m_fragmentShaderModule(VK_NULL_HANDLE), m_geometryShaderModule(VK_NULL_HANDLE), m_tessControlShaderModule(VK_NULL_HANDLE), m_tessEvalShaderModule(VK_NULL_HANDLE),
      m_computeShaderModule(VK_NULL_HANDLE), m_descriptorSetLayouts(0), m_pipelineLayout(VK_NULL_HANDLE), m_descriptorPool(VK_NULL_HANDLE), m_pipelineVertexInputStateCreateInfo(), m_shaderStages(), m_shaderVertexAttributes(),
      m_fragmentOutputs(nullptr), m_vertexInputBindingDescriptions(), m_vertexInputAttributeDescriptions(), m_pipelineCache(), m_computeShaderPipeline(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID())
{
    FN("VulkanShaderProgram::VulkanShaderProgram");
}

VulkanShaderProgram::~VulkanShaderProgram()
{
    FN("VulkanShaderProgram::~VulkanShaderProgram");
    Release();
}

bool VulkanShaderProgram::Initialize(VulkanLogicalDevice *logicalDevice, const std::string &computeShaderSource)
{
    FN("VulkanShaderProgram::Initialize");

    m_logicalDevice = logicalDevice;

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;

    std::vector<uint32_t> computeShaderCompiled = CompileGLSLToSPIRV(computeShaderSource, shaderc_compute_shader);
    if (computeShaderCompiled.empty())
    {
        LOG_ERROR("VulkanShaderProgram: Failed to compile compute shader");
        return false;
    }
    shaderModuleCreateInfo.codeSize = computeShaderCompiled.size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode = computeShaderCompiled.data();

    LOG_TRACE("vkCreateShaderModule %s", m_guid.c_str());
    VkResult result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_computeShaderModule);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    // Create shader stage info for each shader
    VkPipelineShaderStageCreateInfo computeShaderStageInfo = {};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.pNext = nullptr;
    computeShaderStageInfo.flags = 0;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = m_computeShaderModule;
    computeShaderStageInfo.pName = "main";
    computeShaderStageInfo.pSpecializationInfo = nullptr;

    //////////////////////////////////////////
    // Create pipeline layout

    m_shaderPushConstants[ShaderStage::Compute] = FindPushConstants(computeShaderCompiled);
    m_shaderResources[ShaderStage::Compute] = FindShaderResources(computeShaderCompiled);

    std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
    auto populateDescriptorSetLayoutBindings = [&](VulkanShaderResourceMap shaderResources, VkShaderStageFlags stageFlags)
    {
        for (auto it = shaderResources.begin(); it != shaderResources.end(); it++)
        {
            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
            descriptorSetLayoutBinding.binding = it->second->binding;
            switch (it->second->resourceType)
            {
            case ShaderResourceType::Sampler:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                break;
            case ShaderResourceType::CombinedImageSampler:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                break;
            case ShaderResourceType::SampledImage:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::SeparateImage:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::StorageImage:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                break;
            case ShaderResourceType::UniformBuffer:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case ShaderResourceType::StorageBuffer:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            case ShaderResourceType::SubpassInput:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                break;
            case ShaderResourceType::AtomicCounter:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                LOG_ERROR("Unknown resource type");
            }
            descriptorSetLayoutBinding.descriptorCount = 1;
            descriptorSetLayoutBinding.stageFlags = stageFlags;
            descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
            descriptorSetLayoutBindings[it->second->set].push_back(descriptorSetLayoutBinding);
        }
    };

    populateDescriptorSetLayoutBindings(m_shaderResources[ShaderStage::Compute], VK_SHADER_STAGE_COMPUTE_BIT);

    m_descriptorSetLayouts.resize(descriptorSetLayoutBindings.size());
    for (auto it = descriptorSetLayoutBindings.begin(); it != descriptorSetLayoutBindings.end(); it++)
    {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pNext = nullptr;
        descriptorSetLayoutCreateInfo.flags = 0;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(it->second.size());
        descriptorSetLayoutCreateInfo.pBindings = it->second.data();

        if (it->first >= m_descriptorSetLayouts.size())
        {
            LOG_FATAL("VulkanShaderProgram: descriptorSetLayoutBindings index out of range!");
            return false;
        }

        LOG_TRACE("vkCreateDescriptorSetLayout %s", m_guid.c_str());
        result = vkCreateDescriptorSetLayout(m_logicalDevice->GetHandle(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayouts[it->first]);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }
    }

    // Process push constants for pipeline layout creation
    std::vector<VkPushConstantRange> pushConstantRanges;
    auto populatePushConstantRanges = [&](VulkanPushConstantMap shaderPushConstants, VkShaderStageFlags stageFlags)
    {
        for (auto it = shaderPushConstants.begin(); it != shaderPushConstants.end(); it++)
        {
            uint32_t maxPushConstantsSize = m_logicalDevice->GetParentPhysicalDevice()->GetPhysicalDeviceProperties().properties.limits.maxPushConstantsSize;
            if (maxPushConstantsSize < it->second->sizeInBytes)
            {
                LOG_ERROR("VulkanShaderProgram: Push constant size exceeds device limit! Max size: %u, Push constant '%s' size: %u", maxPushConstantsSize, it->second->name.c_str(), it->second->sizeInBytes);
                return false;
            }

            VkPushConstantRange pushConstantRange = {};
            pushConstantRange.stageFlags = stageFlags;
            pushConstantRange.offset = 0;
            pushConstantRange.size = static_cast<uint32_t>(it->second->sizeInBytes);
            pushConstantRanges.push_back(pushConstantRange);

            m_shaderPushConstantsData[ShaderStage::Compute].resize(it->second->sizeInBytes);
            memset(m_shaderPushConstantsData[ShaderStage::Compute].data(), 0, it->second->sizeInBytes);
        }
    };

    populatePushConstantRanges(m_shaderPushConstants[ShaderStage::Compute], VK_SHADER_STAGE_COMPUTE_BIT);

    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    auto populateDescriptorPoolSizes = [&](VulkanShaderResourceMap shaderResources)
    {
        for (auto it = shaderResources.begin(); it != shaderResources.end(); it++)
        {
            VkDescriptorPoolSize descriptorPoolSize = {};
            switch (it->second->resourceType)
            {
            case ShaderResourceType::Sampler:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
                break;
            case ShaderResourceType::CombinedImageSampler:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                break;
            case ShaderResourceType::SampledImage:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::SeparateImage:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::StorageImage:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                break;
            case ShaderResourceType::UniformBuffer:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case ShaderResourceType::StorageBuffer:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            case ShaderResourceType::SubpassInput:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                break;
            case ShaderResourceType::AtomicCounter:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                LOG_ERROR("Unknown resource type");
            }
            descriptorPoolSize.descriptorCount = 1;
            descriptorPoolSizes.push_back(descriptorPoolSize);
        }
    };

    populateDescriptorPoolSizes(m_shaderResources[ShaderStage::Compute]);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = 7000;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

    LOG_TRACE("vkCreateDescriptorPool %s", m_guid.c_str());
    result = vkCreateDescriptorPool(m_logicalDevice->GetHandle(), &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

    LOG_TRACE("vkCreatePipelineLayout %s", m_guid.c_str());
    result = vkCreatePipelineLayout(m_logicalDevice->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    VkComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.stage = computeShaderStageInfo;
    computePipelineCreateInfo.layout = m_pipelineLayout;
    computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    computePipelineCreateInfo.basePipelineIndex = 0;

    LOG_TRACE("vkCreateComputePipelines %s", m_guid.c_str());
    if (vkCreateComputePipelines(m_logicalDevice->GetHandle(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_computeShaderPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create compute pipeline!");
    }

    return true;
}

bool VulkanShaderProgram::Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
{
    FN("VulkanShaderProgram::Initialize");

    return Initialize(logicalDevice, vertexShaderSource, fragmentShaderSource, std::string());
}

bool VulkanShaderProgram::Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource)
{
    FN("VulkanShaderProgram::Initialize");

    return Initialize(logicalDevice, vertexShaderSource, fragmentShaderSource, geometryShaderSource, std::string(), std::string());
}

bool VulkanShaderProgram::Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &tessControlShaderSource, const std::string &tessEvalShaderSource)
{
    FN("VulkanShaderProgram::Initialize");

    return Initialize(logicalDevice, vertexShaderSource, fragmentShaderSource, std::string(), tessControlShaderSource, tessEvalShaderSource);
}

bool VulkanShaderProgram::Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource, const std::string &tessControlShaderSource,
                                     const std::string &tessEvalShaderSource)
{
    FN("VulkanShaderProgram::Initialize");

    m_logicalDevice = logicalDevice;

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;

    std::vector<uint32_t> vertexShaderCompiled = CompileGLSLToSPIRV(vertexShaderSource, shaderc_vertex_shader);
    if (vertexShaderCompiled.empty())
    {
        LOG_ERROR("VulkanShaderProgram: Failed to compile vertex shader");
        return false;
    }
    shaderModuleCreateInfo.codeSize = vertexShaderCompiled.size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode = vertexShaderCompiled.data();

    LOG_TRACE("vkCreateShaderModule %s", m_guid.c_str());
    VkResult result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_vertexShaderModule);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    std::vector<uint32_t> fragmentShaderCompiled = CompileGLSLToSPIRV(fragmentShaderSource, shaderc_fragment_shader);
    if (fragmentShaderCompiled.empty())
    {
        LOG_ERROR("VulkanShaderProgram: Failed to compile fragment shader");
        return false;
    }
    shaderModuleCreateInfo.codeSize = fragmentShaderCompiled.size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode = fragmentShaderCompiled.data();

    LOG_TRACE("vkCreateShaderModule %s", m_guid.c_str());
    result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_fragmentShaderModule);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    if (!geometryShaderSource.empty())
    {
        std::vector<uint32_t> geometryShaderCompiled = CompileGLSLToSPIRV(geometryShaderSource, shaderc_geometry_shader);
        if (geometryShaderCompiled.empty())
        {
            LOG_ERROR("VulkanShaderProgram: Failed to compile geometry shader");
            return false;
        }
        shaderModuleCreateInfo.codeSize = geometryShaderCompiled.size() * sizeof(uint32_t);
        shaderModuleCreateInfo.pCode = geometryShaderCompiled.data();

        LOG_TRACE("vkCreateShaderModule %s", m_guid.c_str());
        result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_geometryShaderModule);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }
    }

    if (!tessControlShaderSource.empty())
    {
        std::vector<uint32_t> tessControlShaderCompiled = CompileGLSLToSPIRV(tessControlShaderSource, shaderc_tess_control_shader);
        if (tessControlShaderCompiled.empty())
        {
            LOG_ERROR("VulkanShaderProgram: Failed to compile tessellation "
                      "control shader");
            return false;
        }
        shaderModuleCreateInfo.codeSize = tessControlShaderCompiled.size() * sizeof(uint32_t);
        shaderModuleCreateInfo.pCode = tessControlShaderCompiled.data();

        LOG_TRACE("vkCreateShaderModule %s", m_guid.c_str());
        result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_tessControlShaderModule);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }
    }

    if (!tessEvalShaderSource.empty())
    {
        std::vector<uint32_t> tessEvalShaderCompiled = CompileGLSLToSPIRV(tessEvalShaderSource, shaderc_tess_evaluation_shader);
        if (tessEvalShaderCompiled.empty())
        {
            LOG_ERROR("VulkanShaderProgram: Failed to compile tessellation "
                      "evaluation shader");
            return false;
        }
        shaderModuleCreateInfo.codeSize = tessEvalShaderCompiled.size() * sizeof(uint32_t);
        shaderModuleCreateInfo.pCode = tessEvalShaderCompiled.data();

        LOG_TRACE("vkCreateShaderModule %s", m_guid.c_str());
        result = vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_tessEvalShaderModule);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }
    }

    // Create shader stage info for each shader
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.pNext = nullptr;
    vertexShaderStageInfo.flags = 0;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = m_vertexShaderModule;
    vertexShaderStageInfo.pName = "main";
    vertexShaderStageInfo.pSpecializationInfo = nullptr;
    m_shaderStages.push_back(vertexShaderStageInfo);

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.pNext = nullptr;
    fragmentShaderStageInfo.flags = 0;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = m_fragmentShaderModule;
    fragmentShaderStageInfo.pName = "main";
    fragmentShaderStageInfo.pSpecializationInfo = nullptr;
    m_shaderStages.push_back(fragmentShaderStageInfo);

    if (m_geometryShaderModule != VK_NULL_HANDLE)
    {
        VkPipelineShaderStageCreateInfo geometryShaderStageInfo = {};
        geometryShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        geometryShaderStageInfo.pNext = nullptr;
        geometryShaderStageInfo.flags = 0;
        geometryShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        geometryShaderStageInfo.module = m_geometryShaderModule;
        geometryShaderStageInfo.pName = "main";
        geometryShaderStageInfo.pSpecializationInfo = nullptr;
        m_shaderStages.push_back(geometryShaderStageInfo);
    }

    if (m_tessControlShaderModule != VK_NULL_HANDLE)
    {
        VkPipelineShaderStageCreateInfo tessControlShaderStageInfo = {};
        tessControlShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        tessControlShaderStageInfo.pNext = nullptr;
        tessControlShaderStageInfo.flags = 0;
        tessControlShaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        tessControlShaderStageInfo.module = m_tessControlShaderModule;
        tessControlShaderStageInfo.pName = "main";
        tessControlShaderStageInfo.pSpecializationInfo = nullptr;
        m_shaderStages.push_back(tessControlShaderStageInfo);
    }

    if (m_tessEvalShaderModule != VK_NULL_HANDLE)
    {
        VkPipelineShaderStageCreateInfo tessEvalShaderStageInfo = {};
        tessEvalShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        tessEvalShaderStageInfo.pNext = nullptr;
        tessEvalShaderStageInfo.flags = 0;
        tessEvalShaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        tessEvalShaderStageInfo.module = m_tessEvalShaderModule;
        tessEvalShaderStageInfo.pName = "main";
        tessEvalShaderStageInfo.pSpecializationInfo = nullptr;
        m_shaderStages.push_back(tessEvalShaderStageInfo);
    }

    //////////////////////////////////////////
    // Create pipeline layout

    m_shaderPushConstants[ShaderStage::Vertex] = FindPushConstants(vertexShaderCompiled);
    m_shaderPushConstants[ShaderStage::Fragment] = FindPushConstants(fragmentShaderCompiled);

    m_shaderResources[ShaderStage::Vertex] = FindShaderResources(vertexShaderCompiled);
    m_shaderResources[ShaderStage::Fragment] = FindShaderResources(fragmentShaderCompiled);

    std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
    auto populateDescriptorSetLayoutBindings = [&](VulkanShaderResourceMap shaderResources, VkShaderStageFlags stageFlags)
    {
        for (auto it = shaderResources.begin(); it != shaderResources.end(); it++)
        {
            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
            descriptorSetLayoutBinding.binding = it->second->binding;
            switch (it->second->resourceType)
            {
            case ShaderResourceType::Sampler:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                break;
            case ShaderResourceType::CombinedImageSampler:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                break;
            case ShaderResourceType::SampledImage:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::SeparateImage:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::StorageImage:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                break;
            case ShaderResourceType::UniformBuffer:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case ShaderResourceType::StorageBuffer:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            case ShaderResourceType::SubpassInput:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                break;
            case ShaderResourceType::AtomicCounter:
                descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                LOG_ERROR("Unknown resource type");
            }
            descriptorSetLayoutBinding.descriptorCount = 1;
            descriptorSetLayoutBinding.stageFlags = stageFlags;
            descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
            descriptorSetLayoutBindings[it->second->set].push_back(descriptorSetLayoutBinding);
        }
    };

    populateDescriptorSetLayoutBindings(m_shaderResources[ShaderStage::Vertex], VK_SHADER_STAGE_VERTEX_BIT);
    populateDescriptorSetLayoutBindings(m_shaderResources[ShaderStage::Fragment], VK_SHADER_STAGE_FRAGMENT_BIT);

    m_descriptorSetLayouts.resize(descriptorSetLayoutBindings.size());
    for (auto it = descriptorSetLayoutBindings.begin(); it != descriptorSetLayoutBindings.end(); it++)
    {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pNext = nullptr;
        descriptorSetLayoutCreateInfo.flags = 0;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(it->second.size());
        descriptorSetLayoutCreateInfo.pBindings = it->second.data();

        if (it->first >= m_descriptorSetLayouts.size())
        {
            LOG_FATAL("VulkanShaderProgram: descriptorSetLayoutBindings index out of range!");
            return false;
        }

        LOG_TRACE("vkCreateDescriptorSetLayout %s", m_guid.c_str());
        result = vkCreateDescriptorSetLayout(m_logicalDevice->GetHandle(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayouts[it->first]);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }
    }

    // Process push constants for pipeline layout creation
    std::vector<VkPushConstantRange> pushConstantRanges;
    auto populatePushConstantRanges = [&](VulkanPushConstantMap shaderPushConstants, VkShaderStageFlags stageFlags)
    {
        for (auto it = shaderPushConstants.begin(); it != shaderPushConstants.end(); it++)
        {
            uint32_t maxPushConstantsSize = m_logicalDevice->GetParentPhysicalDevice()->GetPhysicalDeviceProperties().properties.limits.maxPushConstantsSize;
            if (maxPushConstantsSize < it->second->sizeInBytes)
            {
                LOG_ERROR("VulkanShaderProgram: Push constant size exceeds device limit! Max size: %u, Push constant '%s' size: %u", maxPushConstantsSize, it->second->name.c_str(), it->second->sizeInBytes);
                return false;
            }

            VkPushConstantRange pushConstantRange = {};
            pushConstantRange.stageFlags = stageFlags;
            pushConstantRange.offset = 0;
            pushConstantRange.size = static_cast<uint32_t>(it->second->sizeInBytes);
            pushConstantRanges.push_back(pushConstantRange);

            if (stageFlags == VK_SHADER_STAGE_VERTEX_BIT)
            {
                m_shaderPushConstantsData[ShaderStage::Vertex].resize(it->second->sizeInBytes);
                memset(m_shaderPushConstantsData[ShaderStage::Vertex].data(), 0, it->second->sizeInBytes);
            }
            else if (stageFlags == VK_SHADER_STAGE_FRAGMENT_BIT)
            {
                m_shaderPushConstantsData[ShaderStage::Fragment].resize(it->second->sizeInBytes);
                memset(m_shaderPushConstantsData[ShaderStage::Vertex].data(), 0, it->second->sizeInBytes);
            }
        }
    };

    populatePushConstantRanges(m_shaderPushConstants[ShaderStage::Vertex], VK_SHADER_STAGE_VERTEX_BIT);
    populatePushConstantRanges(m_shaderPushConstants[ShaderStage::Fragment], VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    auto populateDescriptorPoolSizes = [&](VulkanShaderResourceMap shaderResources)
    {
        for (auto it = shaderResources.begin(); it != shaderResources.end(); it++)
        {
            VkDescriptorPoolSize descriptorPoolSize = {};
            switch (it->second->resourceType)
            {
            case ShaderResourceType::Sampler:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
                break;
            case ShaderResourceType::CombinedImageSampler:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                break;
            case ShaderResourceType::SampledImage:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::SeparateImage:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case ShaderResourceType::StorageImage:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                break;
            case ShaderResourceType::UniformBuffer:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case ShaderResourceType::StorageBuffer:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            case ShaderResourceType::SubpassInput:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                break;
            case ShaderResourceType::AtomicCounter:
                descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                LOG_ERROR("Unknown resource type");
            }
            descriptorPoolSize.descriptorCount = 1;
            descriptorPoolSizes.push_back(descriptorPoolSize);
        }
    };

    populateDescriptorPoolSizes(m_shaderResources[ShaderStage::Vertex]);
    populateDescriptorPoolSizes(m_shaderResources[ShaderStage::Fragment]);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = 7000;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

    LOG_TRACE("vkCreateDescriptorPool %s", m_guid.c_str());
    result = vkCreateDescriptorPool(m_logicalDevice->GetHandle(), &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

    LOG_TRACE("vkCreatePipelineLayout %s", m_guid.c_str());
    result = vkCreatePipelineLayout(m_logicalDevice->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    //////////////////////////////////////////
    // Parse vertex attributes

    m_fragmentOutputs = FindFragmentOutputs(fragmentShaderCompiled);
    m_shaderVertexAttributes = FindVertexAttributes(vertexShaderCompiled);

    m_vertexInputBindingDescriptions = CreateVertexInputBindingDescriptions(m_shaderVertexAttributes);
    m_vertexInputAttributeDescriptions = CreateVertexInputAttributeDescriptions(m_shaderVertexAttributes);

    m_pipelineVertexInputStateCreateInfo = {};
    m_pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_pipelineVertexInputStateCreateInfo.pNext = nullptr;
    m_pipelineVertexInputStateCreateInfo.flags = 0;
    m_pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertexInputBindingDescriptions.size());
    m_pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = m_vertexInputBindingDescriptions.data();
    m_pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertexInputAttributeDescriptions.size());
    m_pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = m_vertexInputAttributeDescriptions.data();

    return true;
}

void VulkanShaderProgram::Release()
{
    FN("VulkanShaderProgram::Release");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());

    if (!m_pipelineCache.empty())
    {
        m_pipelineCache.clear();
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyPipelineLayout %s", m_guid.c_str());
        vkDestroyPipelineLayout(m_logicalDevice->GetHandle(), m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    m_shaderStages.clear();

    if (m_vertexShaderModule != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyShaderModule %s", m_guid.c_str());
        vkDestroyShaderModule(m_logicalDevice->GetHandle(), m_vertexShaderModule, nullptr);
        m_vertexShaderModule = VK_NULL_HANDLE;
    }

    if (m_fragmentShaderModule != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyShaderModule %s", m_guid.c_str());
        vkDestroyShaderModule(m_logicalDevice->GetHandle(), m_fragmentShaderModule, nullptr);
        m_fragmentShaderModule = VK_NULL_HANDLE;
    }

    if (m_geometryShaderModule != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyShaderModule %s", m_guid.c_str());
        vkDestroyShaderModule(m_logicalDevice->GetHandle(), m_geometryShaderModule, nullptr);
        m_geometryShaderModule = VK_NULL_HANDLE;
    }

    if (m_tessControlShaderModule != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyShaderModule %s", m_guid.c_str());
        vkDestroyShaderModule(m_logicalDevice->GetHandle(), m_tessControlShaderModule, nullptr);
        m_tessControlShaderModule = VK_NULL_HANDLE;
    }

    if (m_tessEvalShaderModule != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyShaderModule %s", m_guid.c_str());
        vkDestroyShaderModule(m_logicalDevice->GetHandle(), m_tessEvalShaderModule, nullptr);
        m_tessEvalShaderModule = VK_NULL_HANDLE;
    }
}

ShaderVertexAttributePtr VulkanShaderProgram::VGetVertexAttribute(std::string name)
{
    FN("VulkanShaderProgram::VGetVertexAttribute");

    for (auto it = m_shaderVertexAttributes.begin(); it != m_shaderVertexAttributes.end(); it++)
    {
        if (it->second->Name == name)
        {
            return it->second;
        }
    }
    return nullptr;
}

ShaderVertexAttributeMap VulkanShaderProgram::VGetVertexAttributes()
{
    FN("VulkanShaderProgram::VGetVertexAttributes");

    return m_shaderVertexAttributes;
}

int VulkanShaderProgram::VGetFragmentOutputLocation(std::string name)
{
    FN("VulkanShaderProgram::VGetFragmentOutputLocation");

    return (*m_fragmentOutputs)[name];
}

ShaderResourceBindingsPtr VulkanShaderProgram::VCreateResourceBindingObjects()
{
    FN("VulkanShaderProgram::VCreateResourceBindingObjects");

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
    descriptorSetAllocateInfo.pSetLayouts = m_descriptorSetLayouts.data();

    std::vector<VkDescriptorSet> shaderDescriptorSets(m_descriptorSetLayouts.size());

    LOG_TRACE("vkAllocateDescriptorSets %s", m_guid.c_str());
    VkResult result = vkAllocateDescriptorSets(m_logicalDevice->GetHandle(), &descriptorSetAllocateInfo, shaderDescriptorSets.data());
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return nullptr;
    }

    VulkanShaderResourceBindingsPtr shaderBindings = std::make_shared<VulkanShaderResourceBindings>(m_logicalDevice, shaderDescriptorSets);

    for (auto it = m_shaderResources.begin(); it != m_shaderResources.end(); it++)
    {
        shaderBindings->SetShaderResources(it->first, it->second);
    }

    return shaderBindings;
}

void VulkanShaderProgram::VSetPushConstants(const char *name, const void *data, size_t offset, size_t size)
{
    FN("VulkanShaderProgram::VSetPushConstants");

    auto updatePushConstants = [&](VulkanPushConstantMap shaderPushConstants, VkShaderStageFlags stageFlags, const char *_name, const void *_data, uint32_t _size)
    {
        for (auto it = shaderPushConstants.begin(); it != shaderPushConstants.end(); it++)
        {
            if (it->second->name.compare(name) == 0)
            {
                if (stageFlags == VK_SHADER_STAGE_VERTEX_BIT)
                {
                    LOG_ASSERT(size + offset <= m_shaderPushConstantsData[ShaderStage::Vertex].size(), "VulkanShaderProgram: update data size + offset exceeds push constant size");

                    memcpy(m_shaderPushConstantsData[ShaderStage::Vertex].data() + offset, data, size);
                    return;
                }
                else if (stageFlags == VK_SHADER_STAGE_FRAGMENT_BIT)
                {
                    LOG_ASSERT(size + offset <= m_shaderPushConstantsData[ShaderStage::Fragment].size(), "VulkanShaderProgram: update data size + offset exceeds push constant size");

                    memcpy(m_shaderPushConstantsData[ShaderStage::Fragment].data() + offset, data, size);
                    return;
                }
            }
        }
    };

    updatePushConstants(m_shaderPushConstants[ShaderStage::Vertex], VK_SHADER_STAGE_VERTEX_BIT, name, data, size);
    updatePushConstants(m_shaderPushConstants[ShaderStage::Fragment], VK_SHADER_STAGE_FRAGMENT_BIT, name, data, size);
    updatePushConstants(m_shaderPushConstants[ShaderStage::Compute], VK_SHADER_STAGE_COMPUTE_BIT, name, data, size);
}

void VulkanShaderProgram::VSetPushConstants(ShaderStage shaderStage, const void *data, size_t offset, size_t size)
{
    FN("VulkanShaderProgram::VSetPushConstants");
    LOG_ASSERT(size + offset <= m_shaderPushConstantsData[shaderStage].size(), "VulkanShaderProgram: update data size + offset exceeds push constant size");

    memcpy(m_shaderPushConstantsData[shaderStage].data() + offset, data, size);
}

ShaderResourceBindingsPtr VulkanShaderProgram::VCreateComputeResourceBindingObjects()
{
    FN("VulkanShaderProgram::VCreateComputeResourceBindingObjects");

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
    descriptorSetAllocateInfo.pSetLayouts = m_descriptorSetLayouts.data();

    std::vector<VkDescriptorSet> shaderDescriptorSets(m_descriptorSetLayouts.size());

    LOG_TRACE("vkAllocateDescriptorSets %s", m_guid.c_str());
    VkResult result = vkAllocateDescriptorSets(m_logicalDevice->GetHandle(), &descriptorSetAllocateInfo, shaderDescriptorSets.data());
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return nullptr;
    }

    VulkanShaderResourceBindingsPtr shaderBindings = std::make_shared<VulkanShaderResourceBindings>(m_logicalDevice, shaderDescriptorSets);

    for (auto it = m_shaderResources.begin(); it != m_shaderResources.end(); it++)
    {
        shaderBindings->SetShaderResources(it->first, it->second);
    }

    return shaderBindings;
}

void VulkanShaderProgram::VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    FN("VulkanShaderProgram::VDispatch");

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseComputeCommandBuffer();

    LOG_TRACE("vkCmdBindPipeline %s", m_guid.c_str());
    vkCmdBindPipeline(singleTimeCommandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, m_computeShaderPipeline);

    VulkanShaderResourceBindingsPtr vulkanShaderResourceBindings = std::dynamic_pointer_cast<VulkanShaderResourceBindings>(shaderResourceBindings);
    std::vector<VkDescriptorSet> descriptorSets = vulkanShaderResourceBindings->GetDescriptorSets();
    if (descriptorSets.size() > 0)
    {
        LOG_TRACE("vkCmdBindDescriptorSets %s", m_guid.c_str());
        vkCmdBindDescriptorSets(singleTimeCommandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
    }

    LOG_TRACE("vkCmdDispatch %s", m_guid.c_str());
    vkCmdDispatch(singleTimeCommandBuffer->GetHandle(), groupCountX, groupCountY, groupCountZ);

    if (!singleTimeCommandBuffer->EndAndSubmit())
    {
        LOG_ERROR("VulkanShaderProgram: Failed to end and submit single time command buffer");
    }
}

std::string VulkanShaderProgram::GetGUID() const
{
    FN("VulkanShaderProgram::GetGUID");

    return m_guid;
}

void VulkanShaderProgram::NotifyDestruction(VulkanRenderPass *renderPass)
{
    FN("VulkanShaderProgram::NotifyDestruction");

    LOG_ASSERT(renderPass != nullptr, "VulkanShaderProgram: RenderPass is nullptr");

    for (auto it = m_pipelineCache.begin(); it != m_pipelineCache.end();)
    {
        if (it->first.renderPass == renderPass->GetHandle())
        {
            it = m_pipelineCache.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void VulkanShaderProgram::Bind(VulkanCommandBufferPtr commandBuffer, const PrimitiveType &primitiveType, const RenderState &renderState, const Viewport &viewportInput, VulkanRenderPassPtr renderPass,
                               const std::vector<VkDescriptorSet> &descriptorSets)
{
    FN("VulkanShaderProgram::Bind");

    VulkanPipelinePtr pipeline = GetOrCreateGraphicsPipeline(primitiveType, renderState, viewportInput, renderPass);

    LOG_TRACE("vkCmdBindPipeline %s", m_guid.c_str());
    vkCmdBindPipeline(commandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());

    pipeline->SetDynamicStages(commandBuffer->GetHandle(), renderState, viewportInput, renderPass->GetColorAttachmentWidth(), renderPass->GetColorAttachmentHeight());

    if (descriptorSets.size() > 0)
    {
        LOG_TRACE("vkCmdBindDescriptorSets %s", m_guid.c_str());
        vkCmdBindDescriptorSets(commandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
    }

    for (auto it = m_shaderPushConstantsData.begin(); it != m_shaderPushConstantsData.end(); it++)
    {
        std::vector<uint8_t> vertexConstantData = m_shaderPushConstantsData[ShaderStage::Vertex];
        switch (it->first)
        {
        case ShaderStage::Vertex:
            vkCmdPushConstants(commandBuffer->GetHandle(), m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, it->second.size(), it->second.data());
            break;
        case ShaderStage::Fragment:
            vkCmdPushConstants(commandBuffer->GetHandle(), m_pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, it->second.size(), it->second.data());
            break;
        case ShaderStage::Compute:
            vkCmdPushConstants(commandBuffer->GetHandle(), m_pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, it->second.size(), it->second.data());
            break;
        }
    }
}

// =================================================================================================
// PRIVATE METHODS
// =================================================================================================

std::vector<VkPipelineShaderStageCreateInfo> VulkanShaderProgram::GetShaderStages() const
{
    FN("VulkanShaderProgram::GetShaderStages");

    return m_shaderStages;
}

VulkanPipelinePtr VulkanShaderProgram::GetOrCreateGraphicsPipeline(const PrimitiveType &primitiveType, const RenderState &renderState, const Viewport &viewportInput, VulkanRenderPassPtr renderPass)
{
    FN("VulkanShaderProgram::GetOrCreateGraphicsPipeline");

    auto it = m_pipelineCache.find(PipelineKey(primitiveType, renderState, renderPass->GetHandle()));
    if (it != m_pipelineCache.end())
    {
        return it->second;
    }

    VulkanPipelinePtr newPipeline = VulkanPipelinePtr(new VulkanPipeline());
    newPipeline->Initialize(m_logicalDevice, primitiveType, renderState, viewportInput, renderPass, m_pipelineVertexInputStateCreateInfo, m_shaderStages, m_pipelineLayout);
    m_pipelineCache[PipelineKey(primitiveType, renderState, renderPass->GetHandle())] = newPipeline;
    return newPipeline;
}

ShaderVertexAttributeMap VulkanShaderProgram::FindVertexAttributes(const std::vector<uint32_t> &program)
{
    FN("VulkanShaderProgram::FindVertexAttributes");

    spirv_cross::Compiler *compiler = new spirv_cross::Compiler(program);
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    ShaderVertexAttributeMap vertexAttributes;

    LOG_TRACE("Shader Resources: %zu stage inputs found", resources.stage_inputs.size());
    for (const auto &resource : resources.stage_inputs)
    {
        const spirv_cross::SPIRType &type = compiler->get_type(resource.type_id);
        std::string name = compiler->get_name(resource.id);
        uint32_t location = compiler->get_decoration(resource.id, spv::DecorationLocation);

        if (name.empty())
        {
            name = resource.name;
        }

        if (name.rfind("gl_", 0) == 0) // Skip built-in attributes
        {
            continue;
        }

        ShaderVertexAttributeType attributeType = VulkanTypeConverter::ToShaderVertexAttributeType(type);

        LOG_INFO("\tResource ID: %u, \tName: %s, \tLocation: %d", resource.id, name.c_str(), location);
        vertexAttributes.insert(std::make_pair(location, std::make_shared<ShaderVertexAttribute>(location, name, attributeType, type.vecsize)));
    }

    delete compiler;

    return vertexAttributes;
}

VulkanPushConstantMap VulkanShaderProgram::FindPushConstants(const std::vector<uint32_t> &program)
{
    FN("VulkanShaderProgram::FindPushConstants");

    std::unique_ptr<spirv_cross::Compiler> compiler = std::make_unique<spirv_cross::Compiler>(program);
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    VulkanPushConstantMap pushConstantMap;

    // Process push constants
    LOG_TRACE("Shader Resources: %zu push constants found", resources.push_constant_buffers.size());
    for (const auto &resource : resources.push_constant_buffers)
    {
        const spirv_cross::SPIRType &type = compiler->get_type(resource.type_id);
        size_t size = compiler->get_declared_struct_size(type);
        std::string name = compiler->get_name(resource.id);
        if (name.empty())
        {
            name = resource.name;
        }
        LOG_INFO("\tPush Constant ID: %u, \tName: %s, \tSize: %u", resource.id, name.c_str(), size);
        // Push constants do not have a binding, so using a special value or omitting it
        pushConstantMap.insert(std::make_pair(-1, std::make_shared<VulkanPushConstant>(name, size)));
    }

    return pushConstantMap;
}

VulkanShaderResourceMap VulkanShaderProgram::FindShaderResources(const std::vector<uint32_t> &program)
{
    FN("VulkanShaderProgram::FindResources");

    std::unique_ptr<spirv_cross::Compiler> compiler = std::make_unique<spirv_cross::Compiler>(program);
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    VulkanShaderResourceMap shaderResourceMap;

    // Process uniform buffers
    LOG_TRACE("Shader Resources: %zu uniform buffers found", resources.uniform_buffers.size());
    for (const auto &resource : resources.uniform_buffers)
    {
        const spirv_cross::SPIRType &type = compiler->get_type(resource.type_id);
        size_t size = compiler->get_declared_struct_size(type);
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }
        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tSize: %u", resource.id, name.c_str(), set, binding, size);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, size, ShaderResourceType::UniformBuffer)));
    }

    // Process storage buffers
    LOG_TRACE("Shader Resources: %zu storage buffers found", resources.storage_buffers.size());
    for (const auto &resource : resources.storage_buffers)
    {
        const spirv_cross::SPIRType &type = compiler->get_type(resource.type_id);
        size_t size = compiler->get_declared_struct_size(type);
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }
        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tSize: %u", resource.id, name.c_str(), set, binding, size);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, size, ShaderResourceType::StorageBuffer)));
    }

    // Process sampled images
    LOG_TRACE("Shader Resources: %zu sampled images found", resources.sampled_images.size());
    for (const auto &resource : resources.sampled_images)
    {
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }

        // Determine the type of the sampled image
        ShaderResourceType type = ShaderResourceType::SampledImage;
        const spirv_cross::SPIRType &spirType = compiler->get_type(resource.type_id);
        if (spirType.image.dim == spv::Dim2D && spirType.image.sampled == 1)
        {
            type = ShaderResourceType::CombinedImageSampler;
        }
        else if (spirType.basetype == spirv_cross::SPIRType::Sampler)
        {
            type = ShaderResourceType::Sampler;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tType: %d", resource.id, name.c_str(), set, binding, static_cast<int>(type));
        // Size is not relevant for sampled images, setting it to 0
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, 0, type)));
    }

    // Process separate samplers
    LOG_TRACE("Shader Resources: %zu separate samplers found", resources.separate_samplers.size());
    for (const auto &resource : resources.separate_samplers)
    {
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tType: Sampler", resource.id, name.c_str(), set, binding);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, 0, ShaderResourceType::Sampler)));
    }

    // Process separate images
    LOG_TRACE("Shader Resources: %zu separate images found", resources.separate_images.size());
    for (const auto &resource : resources.separate_images)
    {
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tType: SeparateImage", resource.id, name.c_str(), set, binding);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, 0, ShaderResourceType::SeparateImage)));
    }

    // Process storage images
    LOG_TRACE("Shader Resources: %zu storage images found", resources.storage_images.size());
    for (const auto &resource : resources.storage_images)
    {
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tType: StorageImage", resource.id, name.c_str(), set, binding);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, 0, ShaderResourceType::StorageImage)));
    }

    // Process subpass inputs
    LOG_TRACE("Shader Resources: %zu subpass inputs found", resources.subpass_inputs.size());
    for (const auto &resource : resources.subpass_inputs)
    {
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationInputAttachmentIndex);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tType: SubpassInput", resource.id, name.c_str(), set, binding);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, 0, ShaderResourceType::SubpassInput)));
    }

    // Process atomic counters
    LOG_TRACE("Shader Resources: %zu atomic counters found", resources.atomic_counters.size());
    for (const auto &resource : resources.atomic_counters)
    {
        std::string name = compiler->get_name(resource.id);
        uint32_t binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
        uint32_t set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
        if (name.empty())
        {
            name = resource.name;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tSet: %d, \tBinding: %d, \tType: AtomicCounter", resource.id, name.c_str(), set, binding);
        shaderResourceMap.insert(std::make_pair(binding, std::make_shared<VulkanShaderResource>(set, binding, name, 0, ShaderResourceType::AtomicCounter)));
    }

    return shaderResourceMap;
}

VulkanFragmentOutputsPtr VulkanShaderProgram::FindFragmentOutputs(const std::vector<uint32_t> &program)
{
    FN("VulkanShaderProgram::FindFragmentOutputs");

    std::unique_ptr<spirv_cross::Compiler> compiler = std::make_unique<spirv_cross::Compiler>(program);
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    VulkanFragmentOutputsPtr fragmentOutputs = std::make_shared<VulkanFragmentOutputs>();

    LOG_TRACE("Shader Resources: %zu fragment outputs found", resources.stage_outputs.size());
    for (const auto &resource : resources.stage_outputs)
    {
        const spirv_cross::SPIRType &type = compiler->get_type(resource.type_id);
        std::string name = compiler->get_name(resource.id);
        uint32_t location = compiler->get_decoration(resource.id, spv::DecorationLocation);

        if (name.empty())
        {
            name = resource.name;
        }

        if (name.rfind("gl_", 0) == 0) // Skip built-in attributes
        {
            continue;
        }

        LOG_INFO("\tResource ID: %u, \tName: %s, \tLocation: %d", resource.id, name.c_str(), location);
        fragmentOutputs->insert(std::make_pair(name, location));
    }

    return fragmentOutputs;
}

std::vector<VkVertexInputBindingDescription> VulkanShaderProgram::CreateVertexInputBindingDescriptions(const ShaderVertexAttributeMap &shaderVertexAttributes)
{
    FN("VulkanShaderProgram::CreateVertexInputBindingDescriptions");

    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;

    uint32_t binding = 0;
    for (const auto &shaderVertexAttribute : shaderVertexAttributes)
    {
        VkVertexInputBindingDescription vertexInputBindingDescription = {};
        vertexInputBindingDescription.binding = binding++;
        vertexInputBindingDescription.stride = VertexAttributeBindingsSizes::SizeOf(shaderVertexAttribute.second->Type);
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescriptions.push_back(vertexInputBindingDescription);
    }

    return vertexInputBindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VulkanShaderProgram::CreateVertexInputAttributeDescriptions(const ShaderVertexAttributeMap &shaderVertexAttributes)
{
    FN("VulkanShaderProgram::CreateVertexInputAttributeDescriptions");

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

    uint32_t binding = 0;
    for (const auto &shaderVertexAttribute : shaderVertexAttributes)
    {
        VkVertexInputAttributeDescription vertexInputAttributeDescription = {};
        vertexInputAttributeDescription.location = shaderVertexAttribute.second->Location;
        vertexInputAttributeDescription.binding = binding++;
        vertexInputAttributeDescription.format = VulkanTypeConverter::ToVkFormat(shaderVertexAttribute.second->Type);
        vertexInputAttributeDescription.offset = 0;
        vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
    }

    return vertexInputAttributeDescriptions;
}

} // namespace bow