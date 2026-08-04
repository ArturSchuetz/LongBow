#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanColorAttachments.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

typedef enum class VulkanRenderPassState
{
    Invalid,
    Ready,
    Recording,
} VulkanRenderPassState;

class VulkanRenderPass
{
  public:
    VulkanRenderPass(VulkanLogicalDevice *device);
    ~VulkanRenderPass();

    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================

    bool Initialize(VulkanColorAttachmentsPtr &colorAttachments, VulkanTexture2DPtr m_DepthStencilAttachment, bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true);
    void VRelease();

    // =========================================================================

    VulkanRenderPassState GetState() const;
    VkRenderPass GetHandle() const;

    void NotifyBinding(VulkanShaderProgramPtr shaderProgram);

    bool Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue> &clearValues);
    void End(VkCommandBuffer commandBuffer);

    uint32_t GetColorAttachmentHeight() const;
    uint32_t GetColorAttachmentWidth() const;

    bool IsClearColorBuffer() const;
    bool IsClearDepthBuffer() const;
    bool IsClearStencilBuffer() const;

  private:
    // you shall not copy!
    VulkanRenderPass(const VulkanRenderPass &) = delete;
    VulkanRenderPass &operator=(const VulkanRenderPass &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VkRenderPass m_renderPass;
    VulkanRenderPassState m_renderPassState;

    std::unordered_map<std::string, VulkanShaderProgramPtr> m_bindedShaderProgram;

    uint32_t m_colorAttachmentWidth;
    uint32_t m_colorAttachmentHeight;

    VulkanColorAttachmentsPtr m_colorAttachments;
    VulkanTexture2DPtr m_depthStencilAttachment;
    uint32_t m_colorAttachmentCount;

    bool m_clearColorBuffer;
    bool m_clearDepthBuffer;
    bool m_clearStencilBuffer;

    std::string m_guid;
};

} // namespace bow