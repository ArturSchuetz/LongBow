#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>

namespace bow
{

class VulkanTexture2D : public ITexture2D
{
  public:
    VulkanTexture2D(VulkanLogicalDevice *m_device, const Texture2DDescription &description, VkImage &m_image);
    VulkanTexture2D(VulkanLogicalDevice *m_device, const Texture2DDescription &description);
    ~VulkanTexture2D();

    // =========================================================================
    // INIT/RELEASE STUFF:

    bool Initialize();
    void VRelease();

    // =========================================================================
    // Inherited via ITexture2D

    void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    void VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    std::shared_ptr<void> VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) override;
    Texture2DDescription VGetDescription() override;

    // =========================================================================
    // Vulkan specific

    VkImage GetHandle();
    VkImageView GetImageView();
    VkImageLayout GetImageLayout();
    void SetImageLayout(VkImageLayout imageLayout);

    bool IsStencilFormat();
    bool IsDepthFormat();

    static bool IsStencilFormat(VkFormat format);
    static bool IsDepthFormat(VkFormat format);
    void TransitionImageLayout(VkImageLayout newLayout);

  private:
    // you shall not copy!
    VulkanTexture2D(const VulkanTexture2D &) = delete;
    VulkanTexture2D &operator=(const VulkanTexture2D &) = delete;

    void TransitionImageLayout(VkFormat format, VkImageLayout newLayout);

    VulkanLogicalDevice *m_logicalDevice;

    const Texture2DDescription m_description;
    VkImage m_image;
    VkImageView m_imageView;
    VkImageLayout m_currentLayout;
    VkDeviceMemory m_imageMemory;
    bool m_isPresentableImage;

    std::string m_guid;
};

} // namespace bow