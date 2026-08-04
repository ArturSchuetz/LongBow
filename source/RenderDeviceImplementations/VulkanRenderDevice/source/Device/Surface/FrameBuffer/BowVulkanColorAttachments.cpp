#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanColorAttachments.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanColorAttachments::VulkanColorAttachments() : m_count(0), m_Attachments(), m_guid(Utils::GenerateGUID()) { FN("VulkanColorAttachments::VulkanColorAttachments"); }

VulkanColorAttachments::~VulkanColorAttachments() { FN("VulkanColorAttachments::~VulkanColorAttachments"); }

Texture2DPtr VulkanColorAttachments::VGetAttachment(uint32_t index) const
{
    FN("VulkanColorAttachments::VGetAttachment");

    return std::dynamic_pointer_cast<ITexture2D>(m_Attachments.at(index).Texture);
}

VulkanColorAttachmentMap VulkanColorAttachments::GetAttachments() const
{
    FN("VulkanColorAttachments::GetAttachments");

    return m_Attachments;
}

void VulkanColorAttachments::VSetAttachment(uint32_t index, Texture2DPtr texture)
{
    FN("VulkanColorAttachments::VSetAttachment");

    LOG_ASSERT(!((texture != nullptr) && (!texture->VGetDescription().ColorRenderable())), "Texture must be color renderable but the Description.ColorRenderable property is false.");

    if (m_Attachments.find(index) == m_Attachments.end())
        m_Attachments.insert(std::make_pair(index, VulkanColorAttachment()));

    if (m_Attachments.at(index).Texture != texture)
    {
        if ((m_Attachments.at(index).Texture != nullptr) && (texture == nullptr))
        {
            --m_count;
        }
        else if ((m_Attachments.at(index).Texture == nullptr) && (texture != nullptr))
        {
            ++m_count;
        }

        if (texture->VGetDescription().GetTextureFormat() == TextureFormat::Depth24Stencil8 || texture->VGetDescription().GetTextureFormat() == TextureFormat::Depth32fStencil8)
        {
            LOG_ASSERT(false, "Texture format is not supported as color attachment.");
        }

        m_Attachments.at(index).Description = {};
        m_Attachments.at(index).Description.flags = 0;
        m_Attachments.at(index).Description.format = VulkanTypeConverter::ToVkFormat(texture->VGetDescription().GetTextureFormat());
        m_Attachments.at(index).Description.samples = VK_SAMPLE_COUNT_1_BIT;
        m_Attachments.at(index).Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_Attachments.at(index).Description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        m_Attachments.at(index).Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        m_Attachments.at(index).Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        m_Attachments.at(index).Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        m_Attachments.at(index).Description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        m_Attachments.at(index).Reference = {};
        m_Attachments.at(index).Reference.attachment = index;
        m_Attachments.at(index).Reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        m_Attachments.at(index).Texture = std::dynamic_pointer_cast<VulkanTexture2D>(texture);
    }
}

uint32_t VulkanColorAttachments::GetCount() const
{
    FN("VulkanColorAttachments::GetCount");

    return m_count;
}

} // namespace bow