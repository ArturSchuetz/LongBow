#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/IBowShaderResourceBindings.h>

namespace bow
{

class VulkanShaderResourceBindings : public IShaderResourceBindings
{
  public:
    VulkanShaderResourceBindings(VulkanLogicalDevice *logicalDevice, const std::vector<VkDescriptorSet> &descriptorSet);
    ~VulkanShaderResourceBindings();

    void SetShaderResources(ShaderStage shaderStage, const VulkanShaderResourceMap &resources);

    void VSetBuffer(const char *name, UniformBufferPtr uniformBuffer) override;
    void VSetBuffer(const char *name, StorageBufferPtr storageBuffer) override;
    void VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler) override;
    void VSetAccelerationStructure(const char *name, void *accelerationStructure) override;
    void VSetStorageImage(const char *name, Texture2DPtr texture) override;

    std::vector<VkDescriptorSet> GetDescriptorSets();

  private:
    VulkanTexture2DPtr Create1x1WhiteTexture();
    VulkanTextureSamplerPtr CreateDefaultSampler();

    VulkanLogicalDevice *m_logicalDevice;

    std::vector<VkDescriptorSet> m_shaderDescriptorSets;

    std::unordered_map<std::string, VulkanUniformBufferPtr> m_uniformBuffers;
    std::unordered_map<std::string, VulkanStorageBufferPtr> m_storageBuffers;

    VulkanTexture2DPtr m_defaultTexture;
    VulkanTextureSamplerPtr m_defaultSampler;

    std::unordered_map<ShaderStage, VulkanShaderResourceMap> m_shaderResources;

    std::string m_guid;
};

} // namespace bow