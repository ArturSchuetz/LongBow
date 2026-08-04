#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/Device/RayTracing/IBowRayTracingShaderProgram.h>

namespace bow
{

struct DescriptorBindingInfo
{
    uint32_t set;
    uint32_t binding;
    std::string name;
    VkDescriptorType descriptorType;
    VkShaderStageFlags stageFlags;
};

class VulkanRayTracingShaderProgram : public IRayTracingShaderProgram
{
  public:
    VulkanRayTracingShaderProgram(VulkanLogicalDevice *logicalDevice);
    ~VulkanRayTracingShaderProgram();

    bool Initialize(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource, const std::string &intersectionShaderSource,
                    const std::string &callableShaderSource);
    void Release();

    // ===========================================
    // IRayTracingShaderProgram Functions
    // ===========================================

    void VAddDescriptorBinding(uint32_t set, uint32_t binding, const std::string &name, RTDescriptorType type) override;
    bool VBuild() override;
    ShaderResourceBindingsPtr VCreateResourceBindingObjects() override;

    // ===========================================
    // Pipeline access
    // ===========================================

    VkPipeline GetPipeline() const { return m_pipeline; }
    VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

    const VkStridedDeviceAddressRegionKHR &GetRaygenSBTRegion() const { return m_raygenSBTRegion; }
    const VkStridedDeviceAddressRegionKHR &GetMissSBTRegion() const { return m_missSBTRegion; }
    const VkStridedDeviceAddressRegionKHR &GetHitSBTRegion() const { return m_hitSBTRegion; }
    const VkStridedDeviceAddressRegionKHR &GetCallableSBTRegion() const { return m_callableSBTRegion; }

    std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }

  private:
    VulkanLogicalDevice *m_logicalDevice;

    std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() const;

    bool BuildDescriptorSetLayouts(const std::vector<DescriptorBindingInfo> &bindings);
    bool CreatePipelineAndSBT();

    VkShaderModule m_raygenShaderModule;
    VkShaderModule m_anyHitShaderModule;
    VkShaderModule m_closestHitShaderModule;
    VkShaderModule m_missShaderModule;
    VkShaderModule m_intersectionShaderModule;
    VkShaderModule m_callableShaderModule;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> m_shaderGroups;

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<DescriptorBindingInfo> m_descriptorBindings;
    VkDescriptorPool m_descriptorPool;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;

    VulkanBufferPtr m_sbtBuffer;
    VkStridedDeviceAddressRegionKHR m_raygenSBTRegion;
    VkStridedDeviceAddressRegionKHR m_missSBTRegion;
    VkStridedDeviceAddressRegionKHR m_hitSBTRegion;
    VkStridedDeviceAddressRegionKHR m_callableSBTRegion;

    std::unordered_map<ShaderStage, VulkanShaderResourceMap> m_shaderResources;

    std::string m_guid;
};

} // namespace bow
