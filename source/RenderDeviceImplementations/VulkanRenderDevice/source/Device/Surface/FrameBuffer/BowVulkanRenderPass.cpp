#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanRenderPass.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderProgram.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanColorAttachments.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanRenderPass::VulkanRenderPass(VulkanLogicalDevice *device)
    : m_logicalDevice(device), m_renderPass(VK_NULL_HANDLE), m_renderPassState(VulkanRenderPassState::Invalid), m_bindedShaderProgram(), m_colorAttachmentWidth(0), m_colorAttachmentHeight(0), m_colorAttachmentCount(0), m_clearColorBuffer(true),
      m_clearDepthBuffer(true), m_clearStencilBuffer(true), m_guid(Utils::GenerateGUID())
{
    FN("VulkanRenderPass::VulkanRenderPass");
}

VulkanRenderPass::~VulkanRenderPass()
{
    FN("VulkanRenderPass::~VulkanRenderPass");

    VRelease();
}

bool VulkanRenderPass::Initialize(VulkanColorAttachmentsPtr &colorAttachments, VulkanTexture2DPtr depthStencilAttachment, bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer)
{
    FN("VulkanRenderPass::Initialize");
    OPTICK_EVENT();

    LOG_ASSERT(m_renderPassState == VulkanRenderPassState::Invalid, "VulkanRenderPass: Already initialized");

    m_clearColorBuffer = clearColorBuffer;
    m_clearDepthBuffer = clearDepthBuffer;
    m_clearStencilBuffer = clearStencilBuffer;

    std::vector<VkAttachmentDescription> attachmentDescriptions;
    std::vector<VkAttachmentReference> colorAttachmentReferences;

    bool colorAttachmentSet = false;

    for (const auto &attachment : colorAttachments->GetAttachments())
    {
        LOG_ASSERT(attachment.first < colorAttachments->GetCount(), "VulkanFramebuffer: Attachment index out of bounds! The index must be less than the total number of attachments");
        if (attachment.second.Texture->VGetDescription().ColorRenderable())
        {
            if (!colorAttachmentSet)
            {
                if (m_colorAttachmentWidth == 0 && m_colorAttachmentHeight == 0)
                {
                    m_colorAttachmentWidth = attachment.second.Texture->VGetDescription().GetWidth();
                    m_colorAttachmentHeight = attachment.second.Texture->VGetDescription().GetHeight();
                }
                else
                {
                    LOG_ASSERT(attachment.second.Texture->VGetDescription().GetWidth() == m_colorAttachmentWidth && attachment.second.Texture->VGetDescription().GetHeight() == m_colorAttachmentHeight,
                               "VulkanFramebuffer: All attachments must have the same dimensions!");
                }
                colorAttachmentSet = true;
            }
        }

        VkAttachmentDescription attachmentDescription = attachment.second.Description;
        attachmentDescription.loadOp = m_clearColorBuffer ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        if (m_clearColorBuffer == false)
        {
            attachment.second.Texture->TransitionImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }

        attachmentDescription.initialLayout = attachment.second.Texture->GetImageLayout();

        attachmentDescriptions.push_back(attachmentDescription);
        colorAttachmentReferences.push_back(attachment.second.Reference);
    }

    VkAttachmentReference depthStencilReference = {};
    if (depthStencilAttachment != nullptr)
    {
        depthStencilReference.attachment = colorAttachments->GetCount();
        if (depthStencilAttachment)
        {
            if (m_colorAttachmentWidth == 0 && m_colorAttachmentHeight == 0)
            {
                m_colorAttachmentWidth = depthStencilAttachment->VGetDescription().GetWidth();
                m_colorAttachmentHeight = depthStencilAttachment->VGetDescription().GetHeight();
            }
            else
            {
                LOG_ASSERT(depthStencilAttachment->VGetDescription().GetWidth() == m_colorAttachmentWidth && depthStencilAttachment->VGetDescription().GetHeight() == m_colorAttachmentHeight,
                           "VulkanFramebuffer: All attachments must have the same dimensions!");
            }

            if (depthStencilAttachment->IsStencilFormat())
            {
                depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else if (depthStencilAttachment->IsDepthFormat())
            {
                depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            }
        }
        else
        {
            LOG_ERROR("VulkanRenderPass: Depth stencil attachment is nullptr");
            return false;
        }

        VkAttachmentDescription depthStencilAttachmentDescription = {};
        depthStencilAttachmentDescription.flags = 0;
        depthStencilAttachmentDescription.format = VulkanTypeConverter::ToVkFormat(depthStencilAttachment->VGetDescription().GetTextureFormat());
        depthStencilAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        depthStencilAttachmentDescription.loadOp = m_clearDepthBuffer && depthStencilAttachment->IsDepthFormat() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        depthStencilAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthStencilAttachmentDescription.stencilLoadOp = m_clearStencilBuffer && depthStencilAttachment->IsStencilFormat() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        depthStencilAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        if (depthStencilAttachment->IsStencilFormat())
        {
            if (depthStencilAttachment->GetImageLayout() != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                depthStencilAttachment->TransitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
            }

            depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
        else if (depthStencilAttachment->IsDepthFormat())
        {
            if (depthStencilAttachment->GetImageLayout() != VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
            {
                depthStencilAttachment->TransitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
            }

            depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        }
        depthStencilAttachmentDescription.initialLayout = depthStencilAttachment->GetImageLayout();
        attachmentDescriptions.push_back(depthStencilAttachmentDescription);
    }

    VkSubpassDescription subpassDescription = {};
    subpassDescription.flags = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.colorAttachmentCount = colorAttachments->GetCount();
    subpassDescription.pColorAttachments = colorAttachmentReferences.data();
    subpassDescription.pResolveAttachments = nullptr;
    subpassDescription.pDepthStencilAttachment = depthStencilAttachment != nullptr ? &depthStencilReference : nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    m_colorAttachments = colorAttachments;
    m_depthStencilAttachment = depthStencilAttachment;
    m_colorAttachmentCount = attachmentDescriptions.size();

    LOG_TRACE("vkCreateRenderPass %s", m_guid.c_str());
    VkResult result = vkCreateRenderPass(m_logicalDevice->GetHandle(), &renderPassCreateInfo, nullptr, &m_renderPass);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanRenderPass: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    m_renderPassState = VulkanRenderPassState::Ready;
    return true;
}

void VulkanRenderPass::VRelease()
{
    FN("VulkanRenderPass::VRelease");

    for (auto &shaderProgram : m_bindedShaderProgram)
    {
        shaderProgram.second->NotifyDestruction(this);
    }
    m_bindedShaderProgram.clear();

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());
    if (m_renderPass != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyRenderPass %s", m_guid.c_str());
        vkDestroyRenderPass(m_logicalDevice->GetHandle(), m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
        m_renderPassState = VulkanRenderPassState::Invalid;
    }
}

VulkanRenderPassState VulkanRenderPass::GetState() const
{
    FN("VulkanRenderPass::GetState");

    return m_renderPassState;
}

VkRenderPass VulkanRenderPass::GetHandle() const
{
    FN("VulkanRenderPass::GetHandle");

    return m_renderPass;
}

void VulkanRenderPass::NotifyBinding(VulkanShaderProgramPtr shaderProgram)
{
    FN("VulkanRenderPass::NotifyBinding)");

    m_bindedShaderProgram[shaderProgram->GetGUID()] = shaderProgram;
}

bool VulkanRenderPass::Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue> &clearValues)
{
    FN("VulkanRenderPass::Begin");
    OPTICK_EVENT();

    for (const auto &attachment : m_colorAttachments->GetAttachments())
    {
        if (attachment.second.Texture->GetImageLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL || !m_clearColorBuffer)
        {
            attachment.second.Texture->TransitionImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass;
    renderPassBeginInfo.framebuffer = framebuffer;
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    LOG_TRACE("vkCmdBeginRenderPass %s", m_guid.c_str());
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    m_renderPassState = VulkanRenderPassState::Recording;
    return true;
}

void VulkanRenderPass::End(VkCommandBuffer commandBuffer)
{
    FN("VulkanRenderPass::End)");
    OPTICK_EVENT();

    LOG_TRACE("vkCmdEndRenderPass %s", m_guid.c_str());
    vkCmdEndRenderPass(commandBuffer);
    m_renderPassState = VulkanRenderPassState::Ready;

    for (const auto &attachment : m_colorAttachments->GetAttachments())
    {
        attachment.second.Texture->SetImageLayout(attachment.second.Description.finalLayout);
    }
}

uint32_t VulkanRenderPass::GetColorAttachmentHeight() const
{
    FN("VulkanRenderPass::GetColorAttachmentHeight");

    return m_colorAttachmentHeight;
}

uint32_t VulkanRenderPass::GetColorAttachmentWidth() const
{
    FN("VulkanRenderPass::GetColorAttachmentWidth");

    return m_colorAttachmentWidth;
}

bool VulkanRenderPass::IsClearColorBuffer() const
{
    FN("VulkanRenderPass::IsClearColorBuffer");

    return m_clearColorBuffer;
}
bool VulkanRenderPass::IsClearDepthBuffer() const
{
    FN("VulkanRenderPass::IsClearDepthBuffer");

    return m_clearDepthBuffer;
}

bool VulkanRenderPass::IsClearStencilBuffer() const
{
    FN("VulkanRenderPass::IsClearStencilBuffer");

    return m_clearStencilBuffer;
}

} // namespace bow