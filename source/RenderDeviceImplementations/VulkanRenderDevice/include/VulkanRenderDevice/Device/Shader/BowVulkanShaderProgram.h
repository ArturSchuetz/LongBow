#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <VulkanRenderDevice/Device/Shader/BowPipelineKey.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/Shader/IBowComputeShaderProgram.h>
#include <RenderDevice/Device/Shader/IBowShaderProgram.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>
#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

namespace bow
{

struct UpdatePushConstantCommand
{
    UpdatePushConstantCommand() : offset(0), size(0) {}
    UpdatePushConstantCommand(size_t _offset, size_t _size) : offset(_offset), size(_size) {}

    size_t offset;
    size_t size;
};

class VulkanShaderProgram : public IShaderProgram, public IComputeShaderProgram
{
  public:
    VulkanShaderProgram();
    ~VulkanShaderProgram();

    bool Initialize(VulkanLogicalDevice *logicalDevice, const std::string &computeShaderSource);

    bool Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource);

    bool Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource);

    bool Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &tessControlShaderSource, const std::string &tessEvalShaderSource);

    bool Initialize(VulkanLogicalDevice *logicalDevice, const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource, const std::string &tessControlShaderSource,
                    const std::string &tessEvalShaderSource);
    void Release();

    // ===========================================
    // IShaderProgram Functions
    // ===========================================

    ShaderVertexAttributePtr VGetVertexAttribute(std::string name) override;
    ShaderVertexAttributeMap VGetVertexAttributes() override;
    int VGetFragmentOutputLocation(std::string name) override;

    ShaderResourceBindingsPtr VCreateResourceBindingObjects() override;

    void VSetPushConstants(const char *name, const void *data, size_t offset, size_t size) override;
    void VSetPushConstants(ShaderStage shaderStage, const void *data, size_t offset, size_t size) override;

    // ===========================================
    // IComputeShaderProgram Functions
    // ===========================================

    ShaderResourceBindingsPtr VCreateComputeResourceBindingObjects() override;
    void VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    // ===========================================
    // VulkanShaderProgram Functions
    // ===========================================

    std::string GetGUID() const;
    void NotifyDestruction(VulkanRenderPass *renderPass);

    void Bind(VulkanCommandBufferPtr commandBuffer, const PrimitiveType &primitiveType, const RenderState &renderState, const Viewport &viewport, VulkanRenderPassPtr renderPass, const std::vector<VkDescriptorSet> &descriptorSets);

  private:
    // you shall not copy!
    VulkanShaderProgram(const VulkanShaderProgram &) = delete;
    VulkanShaderProgram &operator=(const VulkanShaderProgram &) = delete;

    std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() const;

    VulkanPipelinePtr GetOrCreateGraphicsPipeline(const PrimitiveType &primitiveType, const RenderState &renderState, const Viewport &viewport, VulkanRenderPassPtr renderPass);

    static ShaderVertexAttributeMap FindVertexAttributes(const std::vector<uint32_t> &program);
    static VulkanPushConstantMap FindPushConstants(const std::vector<uint32_t> &program);
    static VulkanShaderResourceMap FindShaderResources(const std::vector<uint32_t> &program);
    static VulkanFragmentOutputsPtr FindFragmentOutputs(const std::vector<uint32_t> &program);

    static std::vector<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptions(const ShaderVertexAttributeMap &shaderVertexAttributes);
    static std::vector<VkVertexInputAttributeDescription> CreateVertexInputAttributeDescriptions(const ShaderVertexAttributeMap &shaderVertexAttributes);

    VulkanLogicalDevice *m_logicalDevice;

    VkShaderModule m_vertexShaderModule;
    VkShaderModule m_fragmentShaderModule;
    VkShaderModule m_geometryShaderModule;
    VkShaderModule m_tessControlShaderModule;
    VkShaderModule m_tessEvalShaderModule;
    VkShaderModule m_computeShaderModule;

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

    VkPipelineLayout m_pipelineLayout;

    VkDescriptorPool m_descriptorPool;

    VkPipelineVertexInputStateCreateInfo m_pipelineVertexInputStateCreateInfo;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    ShaderVertexAttributeMap m_shaderVertexAttributes;

    std::unordered_map<ShaderStage, std::vector<uint8_t>> m_shaderPushConstantsData;
    std::unordered_map<ShaderStage, VulkanPushConstantMap> m_shaderPushConstants;
    std::unordered_map<ShaderStage, VulkanShaderResourceMap> m_shaderResources;

    VulkanFragmentOutputsPtr m_fragmentOutputs;

    std::vector<VkVertexInputBindingDescription> m_vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributeDescriptions;

    std::unordered_map<PipelineKey, VulkanPipelinePtr> m_pipelineCache;
    VkPipeline m_computeShaderPipeline;

    std::string m_guid;
};

} // namespace bow