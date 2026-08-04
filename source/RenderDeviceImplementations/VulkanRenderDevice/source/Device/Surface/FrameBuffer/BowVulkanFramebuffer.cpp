#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanFramebuffer.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanCommandPool.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanColorAttachments.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanRenderPass.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanFramebuffer::VulkanFramebuffer(VulkanLogicalDevice *device)
    : m_logicalDevice(device), m_renderPass(nullptr), m_framebuffer(VK_NULL_HANDLE), m_ColorAttachments(new VulkanColorAttachments()), m_DepthAttachment(nullptr), m_DepthStencilAttachment(nullptr), m_dirty(true), m_guid(Utils::GenerateGUID())
{
    FN("VulkanFramebuffer::VulkanFramebuffer");
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    FN("VulkanFramebuffer::~VulkanFramebuffer");

    VRelease();
}

bool VulkanFramebuffer::Initialize(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer)
{
    FN("VulkanFramebuffer::Initialize");
    OPTICK_EVENT();

    LOG_ASSERT(m_ColorAttachments->GetCount() > 0, "VulkanFramebuffer: No color attachments are set!");

    m_renderPass = VulkanRenderPassPtr(new VulkanRenderPass(m_logicalDevice));

    VulkanTexture2DPtr depthStencilTexture = nullptr;
    if (m_DepthAttachment != nullptr)
        depthStencilTexture = m_DepthAttachment;

    //
    // The depth-stencil attachment overrides the depth attachment:
    //
    //    "Attaching a level of a texture to GL_DEPTH_STENCIL_ATTACHMENT
    //     is equivalent to attaching that level to both the
    //     GL_DEPTH_ATTACHMENT and the GL_STENCIL_ATTACHMENT attachment
    //     points simultaneously."
    //
    // We do not expose just a stencil attachment because TextureFormat
    // does not contain a stencil only format.

    if (m_DepthStencilAttachment != nullptr)
        depthStencilTexture = m_DepthStencilAttachment;

    bool success = m_renderPass->Initialize(m_ColorAttachments, depthStencilTexture, clearColorBuffer, clearDepthBuffer, clearStencilBuffer);
    LOG_ASSERT(success, "VulkanSwapchain: Render pass could not be initialized!");
    if (!success)
        return false;

    int width = -1;
    int height = -1;

    std::vector<VkImageView> attachmentImageViews;
    for (auto &attachment : m_ColorAttachments->GetAttachments())
    {
        Texture2DDescription description = attachment.second.Texture->VGetDescription();
        if (width == -1 && height == -1)
        {
            width = description.GetWidth();
            height = description.GetHeight();
        }
        else
        {
            LOG_ASSERT(description.GetWidth() == width && description.GetHeight() == height, "VulkanFramebuffer: All attachments must have the same dimensions!");
        }

        attachmentImageViews.push_back(attachment.second.Texture->GetImageView());
    }

    if (m_DepthStencilAttachment != nullptr)
    {
        Texture2DDescription description = m_DepthStencilAttachment->VGetDescription();
        if (width == -1 && height == -1)
        {
            width = description.GetWidth();
            height = description.GetHeight();
        }
        else
        {
            LOG_ASSERT(description.GetWidth() == width && description.GetHeight() == height, "VulkanFramebuffer: All attachments must have the same dimensions!");
        }

        attachmentImageViews.push_back(m_DepthStencilAttachment->GetImageView());
    }
    else if (m_DepthAttachment != nullptr)
    {
        Texture2DDescription description = m_DepthAttachment->VGetDescription();
        if (width == -1 && height == -1)
        {
            width = description.GetWidth();
            height = description.GetHeight();
        }
        else
        {
            LOG_ASSERT(description.GetWidth() == width && description.GetHeight() == height, "VulkanFramebuffer: All attachments must have the same dimensions!");
        }
        attachmentImageViews.push_back(m_DepthAttachment->GetImageView());
    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext = nullptr;
    framebufferCreateInfo.flags = 0;
    framebufferCreateInfo.renderPass = m_renderPass->GetHandle();
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentImageViews.size());
    framebufferCreateInfo.pAttachments = attachmentImageViews.data();
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;

    LOG_TRACE("vkCreateFramebuffer %s", m_guid.c_str());
    VkResult result = vkCreateFramebuffer(m_logicalDevice->GetHandle(), &framebufferCreateInfo, nullptr, &m_framebuffer);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanFramebuffer: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_graphicsCommandBuffer = m_logicalDevice->GetGraphicsCommandPool()->AllocateCommandBuffers();
    m_graphicsCommandBufferFence = m_logicalDevice->CreateFence();

    m_renderFinishedSemaphore = m_logicalDevice->CreateSemaphoreA();

    m_dirty = false;
    return true;
}

void VulkanFramebuffer::VRelease()
{
    FN("VulkanFramebuffer::VRelease");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());

    if (m_renderFinishedSemaphore != nullptr)
    {
        m_renderFinishedSemaphore.reset();
        m_renderFinishedSemaphore = nullptr;
    }

    if (m_graphicsCommandBuffer != nullptr)
    {
        m_graphicsCommandBuffer.reset();
        m_graphicsCommandBuffer = nullptr;
    }

    if (m_renderPass != nullptr)
    {
        m_renderPass.reset();
        m_renderPass = nullptr;
    }

    if (m_framebuffer != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyFramebuffer %s", m_guid.c_str());
        vkDestroyFramebuffer(m_logicalDevice->GetHandle(), m_framebuffer, nullptr);
        m_framebuffer = VK_NULL_HANDLE;
    }
}

uint32_t VulkanFramebuffer::GetColorAttachmentsCount() const
{
    FN("VulkanFramebuffer::GetColorAttachmentsCount");

    return m_ColorAttachments->GetCount();
}

void VulkanFramebuffer::UpdateColorAttachmentDescriptions(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer)
{
    FN("VulkanFramebuffer::UpdateColorAttachmentDescriptions");
    OPTICK_EVENT();

    LOG_ASSERT(m_renderPass != nullptr, "VulkanFramebuffer: Render pass is not initialized!");
    if (m_renderPass == nullptr)
    {
        return;
    }

    if (m_renderPass->IsClearColorBuffer() != clearColorBuffer || m_renderPass->IsClearDepthBuffer() != clearDepthBuffer || m_renderPass->IsClearStencilBuffer() != clearStencilBuffer)
    {
        {
            OPTICK_EVENT("bow::VulkanRenderPass::VRelease");
            m_renderPass->VRelease();
        }
        if (m_DepthStencilAttachment != nullptr)
        {
            m_renderPass->Initialize(m_ColorAttachments, m_DepthStencilAttachment, clearColorBuffer, clearDepthBuffer, clearStencilBuffer);
        }
        else
        {
            m_renderPass->Initialize(m_ColorAttachments, m_DepthAttachment, clearColorBuffer, clearDepthBuffer, clearStencilBuffer);
        }
    }
}

Texture2DPtr VulkanFramebuffer::VGetColorAttachment(uint32_t OutputLocation) const
{
    FN("VulkanFramebuffer::VGetColorAttachment");

    return m_ColorAttachments->VGetAttachment(OutputLocation);
}

void VulkanFramebuffer::VSetColorAttachment(uint32_t OutputLocation, Texture2DPtr texture)
{
    FN("VulkanFramebuffer::VSetColorAttachment");

    m_ColorAttachments->VSetAttachment(OutputLocation, texture);
}

void VulkanFramebuffer::VSetColorAttachments(const VulkanColorAttachments &colorAttachments)
{
    FN("VulkanFramebuffer::VSetColorAttachments");

    for (auto &attachment : colorAttachments.GetAttachments())
    {
        m_ColorAttachments->VSetAttachment(attachment.first, attachment.second.Texture);
    }

    m_dirty = true;
}

Texture2DPtr VulkanFramebuffer::VGetDepthAttachment() const
{
    FN("VulkanFramebuffer::VGetDepthAttachment");

    return m_DepthAttachment;
}

void VulkanFramebuffer::VSetDepthAttachment(Texture2DPtr texture)
{
    FN("VulkanFramebuffer::VSetDepthAttachment");

    if (m_DepthAttachment != texture)
    {
        LOG_ASSERT(!((texture != nullptr) && (!texture->VGetDescription().DepthRenderable())), "Texture must be depth renderable but the Description.DepthRenderable property is false.");
        m_DepthAttachment = std::dynamic_pointer_cast<VulkanTexture2D>(texture);

        m_dirty = true;
    }
}

Texture2DPtr VulkanFramebuffer::VGetDepthStencilAttachment() const
{
    FN("VulkanFramebuffer::VGetDepthStencilAttachment");

    return m_DepthStencilAttachment;
}

void VulkanFramebuffer::VSetDepthStencilAttachment(Texture2DPtr texture)
{
    FN("VulkanFramebuffer::VSetDepthStencilAttachment");

    if (m_DepthStencilAttachment != texture)
    {
        LOG_ASSERT(!((texture != nullptr) && (!texture->VGetDescription().DepthStencilRenderable())), "Texture must be depth/stencil renderable but the Description.DepthStencilRenderable property is false.");
        m_DepthStencilAttachment = std::dynamic_pointer_cast<VulkanTexture2D>(texture);

        m_dirty = true;
    }
}

VkFramebuffer VulkanFramebuffer::GetHandle() const
{
    FN("VulkanFramebuffer::GetHandle");

    return m_framebuffer;
}

VulkanRenderPassPtr VulkanFramebuffer::GetRenderPass() const
{
    FN("VulkanFramebuffer::GetRenderPass");

    return m_renderPass;
}

VulkanCommandBufferPtr VulkanFramebuffer::GetGraphicsCommandBuffer() const
{
    FN("VulkanFramebuffer::GetGraphicsCommandBuffer");

    return m_graphicsCommandBuffer;
}

VulkanSemaphorePtr VulkanFramebuffer::GetRenderFinishedSemaphore() const
{
    FN("VulkanFramebuffer::GetRenderFinishedSemaphore");

    return m_renderFinishedSemaphore;
}

bool VulkanFramebuffer::IsDirty() const
{
    FN("VulkanFramebuffer::IsDirty");

    return m_dirty;
}

bool VulkanFramebuffer::IsInitialized() const
{
    FN("VulkanFramebuffer::IsInitialized");

    return m_framebuffer != VK_NULL_HANDLE;
}

uint32_t VulkanFramebuffer::GetWidth() const
{
    FN("VulkanFramebuffer::GetWidth");

    for (auto &attachment : m_ColorAttachments->GetAttachments())
    {
        return attachment.second.Texture->VGetDescription().GetWidth();
    }

    if (m_DepthAttachment != nullptr)
    {
        m_DepthAttachment->VGetDescription().GetWidth();
    }

    if (m_DepthStencilAttachment != nullptr)
    {
        return m_DepthStencilAttachment->VGetDescription().GetWidth();
    }
}

uint32_t VulkanFramebuffer::GetHeight() const
{
    FN("VulkanFramebuffer::GetHeight");

    for (auto &attachment : m_ColorAttachments->GetAttachments())
    {
        return attachment.second.Texture->VGetDescription().GetHeight();
    }

    if (m_DepthAttachment != nullptr)
    {
        m_DepthAttachment->VGetDescription().GetHeight();
    }

    if (m_DepthStencilAttachment != nullptr)
    {
        return m_DepthStencilAttachment->VGetDescription().GetHeight();
    }
}

} // namespace bow