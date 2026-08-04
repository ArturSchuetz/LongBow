#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/BowRenderState.h>

namespace bow
{

class VulkanPipeline
{
  public:
    VulkanPipeline();
    ~VulkanPipeline();

    bool Initialize(VulkanLogicalDevice *logicalDevice, const PrimitiveType &primitiveType, const RenderState &renderState, const Viewport &viewportInput, VulkanRenderPassPtr renderPass,
                    const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages, VkPipelineLayout pipelineLayout);
    void Release();

    VkPipeline GetHandle() const
    {
        FN("VulkanPipeline::GetHandle");

        return m_pipeline;
    }

    void SetDynamicStages(VkCommandBuffer commandBuffer, const RenderState &renderState, const Viewport &viewport, uint32_t bufferWidth, uint32_t bufferHeight);

  private:
    // you shall not copy!
    VulkanPipeline(const VulkanPipeline &) = delete;
    VulkanPipeline &operator=(const VulkanPipeline &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VkPipeline m_pipeline;

    PrimitiveType m_primitiveType;
    RenderState m_renderState;
    Viewport m_viewport;

    std::string m_guid;
};

} // namespace bow