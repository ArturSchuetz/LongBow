#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanColorAttachments.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/FrameBuffer/IBowFramebuffer.h>

namespace bow
{

class VulkanFramebuffer : public IFramebuffer
{
  public:
    VulkanFramebuffer(VulkanLogicalDevice *device);
    ~VulkanFramebuffer();

    // =========================================================================
    // Init/Release stuff

    bool Initialize(bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true);
    void VRelease();

    // =========================================================================
    // Inherited via IFramebuffer

    Texture2DPtr VGetColorAttachment(uint32_t OutputLocation) const;
    void VSetColorAttachment(uint32_t OutputLocation, Texture2DPtr texture);
    void VSetColorAttachments(const VulkanColorAttachments &colorAttachments);

    Texture2DPtr VGetDepthAttachment() const;
    void VSetDepthAttachment(Texture2DPtr texture);

    Texture2DPtr VGetDepthStencilAttachment() const;
    void VSetDepthStencilAttachment(Texture2DPtr texture);

    // =========================================================================

    VkFramebuffer GetHandle() const;

    VulkanRenderPassPtr GetRenderPass() const;

    VulkanCommandBufferPtr GetGraphicsCommandBuffer() const;

    VulkanSemaphorePtr GetRenderFinishedSemaphore() const;

    bool IsDirty() const;
    bool IsInitialized() const;

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

    uint32_t GetColorAttachmentsCount() const;
    void UpdateColorAttachmentDescriptions(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer);

  private:
    // you shall not copy!
    VulkanFramebuffer(const VulkanFramebuffer &) = delete;
    VulkanFramebuffer &operator=(const VulkanFramebuffer &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VulkanRenderPassPtr m_renderPass;

    VulkanCommandBufferPtr m_graphicsCommandBuffer;
    VulkanFencePtr m_graphicsCommandBufferFence;

    VulkanSemaphorePtr m_renderFinishedSemaphore;

    VkFramebuffer m_framebuffer;

    VulkanColorAttachmentsPtr m_ColorAttachments;
    VulkanTexture2DPtr m_DepthAttachment;
    VulkanTexture2DPtr m_DepthStencilAttachment;

    bool m_dirty;

    std::string m_guid;
};

} // namespace bow