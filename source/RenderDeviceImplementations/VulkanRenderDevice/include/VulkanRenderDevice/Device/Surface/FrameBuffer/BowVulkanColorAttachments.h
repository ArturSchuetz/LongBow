#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/FrameBuffer/IBowColorAttachments.h>

namespace bow
{

struct VulkanColorAttachment
{
    VulkanColorAttachment() : Description(), Reference(), Texture(nullptr) {}

    VkAttachmentDescription Description;
    VkAttachmentReference Reference;
    VulkanTexture2DPtr Texture;
};

class VulkanColorAttachments : public IColorAttachments
{
  public:
    VulkanColorAttachments();
    ~VulkanColorAttachments();

    // =========================================================================
    // Inherited via IColorAttachments

    Texture2DPtr VGetAttachment(uint32_t index) const;
    VulkanColorAttachmentMap GetAttachments() const;
    void VSetAttachment(uint32_t index, Texture2DPtr texture);

    uint32_t GetCount() const;

  private:
    // you shall not copy!
    VulkanColorAttachments(const VulkanColorAttachments &) = delete;
    VulkanColorAttachments &operator=(const VulkanColorAttachments &) = delete;

    uint32_t m_count;
    VulkanColorAttachmentMap m_Attachments;

    std::string m_guid;
};

} // namespace bow