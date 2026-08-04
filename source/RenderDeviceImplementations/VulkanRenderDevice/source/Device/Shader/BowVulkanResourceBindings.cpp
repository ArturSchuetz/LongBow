#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResourceBindings.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanTopLevelAccelerationStructure.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanStorageBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanUniformBuffer.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResource.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTextureSampler.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanShaderResourceBindings::VulkanShaderResourceBindings(VulkanLogicalDevice *logicalDevice, const std::vector<VkDescriptorSet> &descriptorSet)
    : m_logicalDevice(logicalDevice), m_shaderDescriptorSets(descriptorSet), m_uniformBuffers(), m_storageBuffers(), m_guid(Utils::GenerateGUID())
{
    FN("VulkanShaderResourceBindings::VulkanShaderResourceBindings");
}

VulkanShaderResourceBindings::~VulkanShaderResourceBindings() { FN("VulkanShaderResourceBindings::~VulkanShaderResourceBindings"); }

void VulkanShaderResourceBindings::SetShaderResources(ShaderStage shaderStage, const VulkanShaderResourceMap &resources)
{
    FN("VulkanShaderResourceBindings::SetShaderResources");

    m_shaderResources[shaderStage] = resources;

    if (!m_defaultTexture)
        m_defaultTexture = Create1x1WhiteTexture();
    if (!m_defaultSampler)
        m_defaultSampler = CreateDefaultSampler();

    for (auto it = m_shaderResources[shaderStage].begin(); it != m_shaderResources[shaderStage].end(); it++)
    {
        if (it->second->resourceType == ShaderResourceType::SampledImage)
        {
            VSetTexture(it->second->name.c_str(), m_defaultTexture, m_defaultSampler);
        }
        else if (it->second->resourceType == ShaderResourceType::CombinedImageSampler)
        {
            VSetTexture(it->second->name.c_str(), m_defaultTexture, m_defaultSampler);
        }
        else if (it->second->resourceType == ShaderResourceType::Sampler)
        {
            VSetTexture(it->second->name.c_str(), m_defaultTexture, m_defaultSampler);
        }
    }
}

void VulkanShaderResourceBindings::VSetBuffer(const char *name, UniformBufferPtr uniformBuffer)
{
    FN("VulkanShaderResourceBindings::VSetBuffer");

    auto updateUniformMemory = [&](VulkanShaderResourceMap shaderResources)
    {
        for (auto it = shaderResources.begin(); it != shaderResources.end(); it++)
        {
            if (it->second->name == name)
            {
                if (it->second->resourceType == ShaderResourceType::UniformBuffer)
                {
                    VulkanUniformBufferPtr vulkanUniformBufferPtr = std::static_pointer_cast<VulkanUniformBuffer>(uniformBuffer);
                    m_uniformBuffers[it->second->name] = vulkanUniformBufferPtr;

                    VkPhysicalDeviceProperties2 deviceProperties = m_logicalDevice->GetParentPhysicalDevice()->GetPhysicalDeviceProperties();
                    VkDeviceSize alignment = deviceProperties.properties.limits.minUniformBufferOffsetAlignment;

                    VkDescriptorBufferInfo bufferInfo = {};
                    bufferInfo.buffer = vulkanUniformBufferPtr->GetHandle();
                    bufferInfo.offset = 0;
                    bufferInfo.range = static_cast<VkDeviceSize>(vulkanUniformBufferPtr->VGetSizeInBytes());

                    VkWriteDescriptorSet writeDescriptorSet = {};
                    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    writeDescriptorSet.pNext = nullptr;
                    writeDescriptorSet.dstSet = m_shaderDescriptorSets[it->second->set];
                    writeDescriptorSet.dstBinding = it->second->binding;
                    writeDescriptorSet.dstArrayElement = 0;
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    writeDescriptorSet.descriptorCount = 1;
                    writeDescriptorSet.pBufferInfo = &bufferInfo;
                    writeDescriptorSet.pImageInfo = nullptr;
                    writeDescriptorSet.pTexelBufferView = nullptr;

                    LOG_TRACE("vkUpdateDescriptorSets %s", m_guid.c_str());
                    vkUpdateDescriptorSets(m_logicalDevice->GetHandle(), 1, &writeDescriptorSet, 0, nullptr);
                }
                else
                {
                    LOG_ERROR("Unknown resource type");
                }
                return;
            }
        }
    };

    for (auto it = m_shaderResources.begin(); it != m_shaderResources.end(); it++)
    {
        updateUniformMemory(it->second);
    }
}

void VulkanShaderResourceBindings::VSetBuffer(const char *name, StorageBufferPtr storageBuffer)
{
    FN("VulkanShaderResourceBindings::VSetBuffer");

    auto updateStorageMemory = [&](VulkanShaderResourceMap shaderStorages)
    {
        for (auto it = shaderStorages.begin(); it != shaderStorages.end(); it++)
        {
            if (it->second->name == name)
            {
                if (it->second->resourceType == ShaderResourceType::StorageBuffer)
                {
                    VulkanStorageBufferPtr vulkanStorageBufferPtr = std::static_pointer_cast<VulkanStorageBuffer>(storageBuffer);
                    m_storageBuffers[it->second->name] = vulkanStorageBufferPtr;

                    VkPhysicalDeviceProperties2 deviceProperties = m_logicalDevice->GetParentPhysicalDevice()->GetPhysicalDeviceProperties();
                    VkDeviceSize alignment = deviceProperties.properties.limits.minStorageBufferOffsetAlignment;

                    VkDescriptorBufferInfo bufferInfo = {};
                    bufferInfo.buffer = vulkanStorageBufferPtr->GetHandle();
                    bufferInfo.offset = 0;
                    bufferInfo.range = static_cast<VkDeviceSize>(vulkanStorageBufferPtr->VGetSizeInBytes());

                    VkWriteDescriptorSet writeDescriptorSet = {};
                    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    writeDescriptorSet.pNext = nullptr;
                    writeDescriptorSet.dstSet = m_shaderDescriptorSets[it->second->set];
                    writeDescriptorSet.dstBinding = it->second->binding;
                    writeDescriptorSet.dstArrayElement = 0;
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    writeDescriptorSet.descriptorCount = 1;
                    writeDescriptorSet.pBufferInfo = &bufferInfo;
                    writeDescriptorSet.pImageInfo = nullptr;
                    writeDescriptorSet.pTexelBufferView = nullptr;

                    LOG_TRACE("vkUpdateDescriptorSets %s", m_guid.c_str());
                    vkUpdateDescriptorSets(m_logicalDevice->GetHandle(), 1, &writeDescriptorSet, 0, nullptr);
                }
                else
                {
                    LOG_ERROR("VulkanShaderProgram: Unknown Uniform type");
                }
                return;
            }
        }
    };

    for (auto it = m_shaderResources.begin(); it != m_shaderResources.end(); it++)
    {
        updateStorageMemory(it->second);
    }
}

void VulkanShaderResourceBindings::VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler)
{
    FN("VulkanShaderResourceBindings::VSetTexture");

    auto updateDescriptorSet = [&](const VulkanShaderResourceMap &shaderResources, VulkanTexture2DPtr vulkanTexture2DPtr, VulkanTextureSamplerPtr vulkanTextureSamplerPtr)
    {
        for (auto it = shaderResources.begin(); it != shaderResources.end(); it++)
        {
            if (it->second->name == name)
            {
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = vulkanTexture2DPtr->GetImageLayout();
                imageInfo.imageView = vulkanTexture2DPtr->GetImageView();
                imageInfo.sampler = vulkanTextureSamplerPtr->GetHandle();

                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.pNext = nullptr;
                writeDescriptorSet.dstSet = m_shaderDescriptorSets[it->second->set];
                writeDescriptorSet.dstBinding = it->second->binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                switch (it->second->resourceType)
                {
                case ShaderResourceType::Sampler:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                    break;
                case ShaderResourceType::CombinedImageSampler:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                case ShaderResourceType::SampledImage:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    break;
                case ShaderResourceType::SeparateImage:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    break;
                case ShaderResourceType::StorageImage:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    break;
                case ShaderResourceType::UniformBuffer:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;
                case ShaderResourceType::StorageBuffer:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                case ShaderResourceType::SubpassInput:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                    break;
                case ShaderResourceType::AtomicCounter:
                    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                default:
                    LOG_ERROR("Unknown resource type");
                }
                writeDescriptorSet.pImageInfo = &imageInfo;

                LOG_TRACE("vkUpdateDescriptorSets %s", m_guid.c_str());
                vkUpdateDescriptorSets(m_logicalDevice->GetHandle(), 1, &writeDescriptorSet, 0, nullptr);
                return;
            }
        }
    };

    for (auto it = m_shaderResources.begin(); it != m_shaderResources.end(); it++)
    {
        updateDescriptorSet(it->second, std::static_pointer_cast<VulkanTexture2D>(texture), std::static_pointer_cast<VulkanTextureSampler>(sampler));
    }
}

void VulkanShaderResourceBindings::VSetAccelerationStructure(const char *name, void *accelerationStructure)
{
    FN("VulkanShaderResourceBindings::VSetAccelerationStructure");

    VulkanTopLevelAccelerationStructure *tlas = static_cast<VulkanTopLevelAccelerationStructure *>(accelerationStructure);
    VkAccelerationStructureKHR as = tlas->GetHandle();

    for (auto &stageResources : m_shaderResources)
    {
        for (auto &res : stageResources.second)
        {
            if (res.second->name == name && res.second->resourceType == ShaderResourceType::AccelerationStructure)
            {
                VkWriteDescriptorSetAccelerationStructureKHR asInfo = {};
                asInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
                asInfo.accelerationStructureCount = 1;
                asInfo.pAccelerationStructures = &as;

                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.pNext = &asInfo;
                writeDescriptorSet.dstSet = m_shaderDescriptorSets[res.second->set];
                writeDescriptorSet.dstBinding = res.second->binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                writeDescriptorSet.descriptorCount = 1;

                vkUpdateDescriptorSets(m_logicalDevice->GetHandle(), 1, &writeDescriptorSet, 0, nullptr);
                return;
            }
        }
    }

    LOG_ERROR("Acceleration structure resource '%s' not found", name);
}

void VulkanShaderResourceBindings::VSetStorageImage(const char *name, Texture2DPtr texture)
{
    FN("VulkanShaderResourceBindings::VSetStorageImage");

    VulkanTexture2DPtr vulkanTexture = std::static_pointer_cast<VulkanTexture2D>(texture);

    // Transition to GENERAL layout if needed
    if (vulkanTexture->GetImageLayout() != VK_IMAGE_LAYOUT_GENERAL)
    {
        vulkanTexture->TransitionImageLayout(VK_IMAGE_LAYOUT_GENERAL);
    }

    for (auto &stageResources : m_shaderResources)
    {
        for (auto &res : stageResources.second)
        {
            if (res.second->name == name && res.second->resourceType == ShaderResourceType::StorageImage)
            {
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                imageInfo.imageView = vulkanTexture->GetImageView();
                imageInfo.sampler = VK_NULL_HANDLE;

                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.dstSet = m_shaderDescriptorSets[res.second->set];
                writeDescriptorSet.dstBinding = res.second->binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(m_logicalDevice->GetHandle(), 1, &writeDescriptorSet, 0, nullptr);
                return;
            }
        }
    }

    LOG_ERROR("Storage image resource '%s' not found", name);
}

std::vector<VkDescriptorSet> VulkanShaderResourceBindings::GetDescriptorSets()
{
    FN("VulkanShaderResourceBindings::GetHandle");

    return m_shaderDescriptorSets;
}

VulkanTexture2DPtr VulkanShaderResourceBindings::Create1x1WhiteTexture()
{
    FN("VulkanShaderResourceBindings::Create1x1WhiteTexture");

    VulkanTexture2DPtr defaultDiffuseTexture = VulkanTexture2DPtr(new VulkanTexture2D(m_logicalDevice, Texture2DDescription(1, 1, TextureFormat::RedGreenBlueAlpha8, false)));
    defaultDiffuseTexture->Initialize();
    uint32_t whitePixel = 0xffffffff;
    defaultDiffuseTexture->VCopyFromSystemMemory(reinterpret_cast<uint8_t *>(&whitePixel), 1, 1, bow::ImageFormat::RedGreenBlueAlpha, bow::ImageDatatype::Byte, 4);
    return defaultDiffuseTexture;
}

VulkanTextureSamplerPtr VulkanShaderResourceBindings::CreateDefaultSampler()
{
    FN("VulkanShaderResourceBindings::CreateDefaultSampler");

    VulkanTextureSamplerPtr vulkanSampler = VulkanTextureSamplerPtr(new VulkanTextureSampler(bow::TextureMinificationFilter::Linear, bow::TextureMagnificationFilter::Linear, bow::TextureWrap::Repeat, bow::TextureWrap::Repeat));
    vulkanSampler->Initialize(m_logicalDevice);
    return vulkanSampler;
}

} // namespace bow