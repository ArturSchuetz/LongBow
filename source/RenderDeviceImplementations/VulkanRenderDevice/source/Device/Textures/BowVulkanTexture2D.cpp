#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{
VulkanTexture2D::VulkanTexture2D(VulkanLogicalDevice *device, const Texture2DDescription &description, VkImage &image)
    : m_logicalDevice(device), m_description(description), m_image(image), m_imageView(VK_NULL_HANDLE), m_currentLayout(VK_IMAGE_LAYOUT_UNDEFINED), m_imageMemory(VK_NULL_HANDLE), m_isPresentableImage(true), m_guid(Utils::GenerateGUID())
{
    FN("VulkanTexture2D::VulkanTexture2D");
}

VulkanTexture2D::VulkanTexture2D(VulkanLogicalDevice *device, const Texture2DDescription &description)
    : m_logicalDevice(device), m_description(description), m_image(VK_NULL_HANDLE), m_imageView(VK_NULL_HANDLE), m_currentLayout(VK_IMAGE_LAYOUT_UNDEFINED), m_imageMemory(VK_NULL_HANDLE), m_isPresentableImage(false), m_guid(Utils::GenerateGUID())
{
    FN("VulkanTexture2D::VulkanTexture2D");
}

VulkanTexture2D::~VulkanTexture2D()
{
    FN("VulkanTexture2D::~VulkanTexture2D");

    VRelease();
}

bool VulkanTexture2D::Initialize()
{
    FN("VulkanTexture2D::Initialize");

    LOG_ASSERT(!(m_description.GetWidth() <= 0), "description.Width must be greater than zero.");
    LOG_ASSERT(!(m_description.GetHeight() <= 0), "description.Height must be greater than zero.");

    if (m_description.GenerateMipmaps())
    {
        LOG_ASSERT(IsPowerOfTwo(m_description.GetWidth()), "When description.GenerateMipmaps is true, the width must be a power of two.");
        LOG_ASSERT(IsPowerOfTwo(m_description.GetHeight()), "When description.GenerateMipmaps is true, the height must be a power of two.");
    }

    VkFormat format = VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat());
    if (!m_logicalDevice->GetParentPhysicalDevice()->IsFormatSupported(format, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
    {
        std::vector<VkFormat> fallbackFormats = {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM};
        format = m_logicalDevice->GetParentPhysicalDevice()->FindSupportedFormat(fallbackFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
        LOG_WARNING("VulkanTexture2D: Falling back to supported format: %s", VulkanTypeConverter::ToString(format).c_str());
    }

    if (format == VK_FORMAT_UNDEFINED)
    {
        LOG_ERROR("VulkanTexture2D: Unsupported texture format");
        return false;
    }

    if (m_image == VK_NULL_HANDLE)
    {
        m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = 0;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent.width = m_description.GetWidth();
        imageCreateInfo.extent.height = m_description.GetHeight();
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = m_description.GenerateMipmaps() ? (static_cast<uint32_t>(std::floor(std::log2(std::max(m_description.GetWidth(), m_description.GetHeight())))) + 1) : 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (m_description.ColorRenderable())
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (m_description.DepthRenderable() || m_description.DepthStencilRenderable())
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = 0;
        imageCreateInfo.pQueueFamilyIndices = nullptr;
        imageCreateInfo.initialLayout = m_currentLayout;

        LOG_TRACE("vkCreateImage %s", m_guid.c_str());
        VkResult result = vkCreateImage(m_logicalDevice->GetHandle(), &imageCreateInfo, nullptr, &m_image);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanTexture2D: %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_logicalDevice->GetHandle(), m_image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = m_logicalDevice->GetParentPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vkAllocateMemory(m_logicalDevice->GetHandle(), &allocInfo, nullptr, &m_imageMemory);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanTexture2D: Failed to allocate image memory! %s", VulkanTypeConverter::ToString(result).c_str());
            return false;
        }

        vkBindImageMemory(m_logicalDevice->GetHandle(), m_image, m_imageMemory, 0);
    }

    return true;
}

void VulkanTexture2D::VRelease()
{
    FN("VulkanTexture2D::VRelease");

    if (!m_isPresentableImage)
    {
        if (m_imageMemory != VK_NULL_HANDLE)
        {
            LOG_TRACE("vkFreeMemory %s", m_guid.c_str());
            vkFreeMemory(m_logicalDevice->GetHandle(), m_imageMemory, nullptr);
            m_imageMemory = VK_NULL_HANDLE;
        }

        if (m_imageView != VK_NULL_HANDLE)
        {
            LOG_TRACE("vkDestroyImageView %s", m_guid.c_str());
            vkDestroyImageView(m_logicalDevice->GetHandle(), m_imageView, nullptr);
            m_imageView = VK_NULL_HANDLE;
        }

        if (m_image != VK_NULL_HANDLE)
        {
            LOG_TRACE("vkDestroyImage %s", m_guid.c_str());
            vkDestroyImage(m_logicalDevice->GetHandle(), m_image, nullptr);
            m_image = VK_NULL_HANDLE;
        }
    }
}

void VulkanTexture2D::VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    FN("VulkanTexture2D::VCopyFromBuffer");
    OPTICK_EVENT();

    LOG_ASSERT(!(xOffset < 0), "xOffset must be greater than or equal to zero.");
    LOG_ASSERT(!(yOffset < 0), "yOffset must be greater than or equal to zero.");
    LOG_ASSERT(!((xOffset + width) > m_description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
    LOG_ASSERT(!((yOffset + height) > m_description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

    LOG_FATAL("VulkanTexture2D: CopyFromBuffer is not implemented yet");

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;      // Adjust this based on your pixelBuffer's internal offset
    region.bufferRowLength = 0;   // Tightly packed
    region.bufferImageHeight = 0; // Tightly packed

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {xOffset, yOffset, 0};
    region.imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    VerifyRowAlignment(rowAlignment);
    /*
    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), oldLayout, newLayout);

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseTransferCommandBuffer();

    LOG_TRACE("vkCmdCopyBufferToImage %s", m_guid.c_str());
    vkCmdCopyBufferToImage(singleTimeCommandBuffer->GetHandle(), pixelBuffer->GetHandle(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    if (!singleTimeCommandBuffer->EndAndSubmit(m_logicalDevice->GetTransferQueue()))
    {
        LOG_FATAL("VulkanTexture2D: Failed to end and submit single time command buffer");
    }

    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), newLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    */
}

void VulkanTexture2D::VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    FN("VulkanTexture2D::VCopyFromSystemMemory");
    OPTICK_EVENT();

    LOG_ASSERT(!((width) > m_description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
    LOG_ASSERT(!((height) > m_description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

    VerifyRowAlignment(rowAlignment);

    size_t numberOfChannels = NumberOfChannels(format);
    size_t sizeInBytes = SizeInBytes(dataType);

    VkDeviceSize imageSize = width * height * numberOfChannels * sizeInBytes;
    VulkanBufferPtr stagingBuffer = VulkanBufferPtr(new VulkanBuffer(m_logicalDevice));

    bool success = stagingBuffer->Initialize(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (!success)
    {
        LOG_FATAL("VulkanTexture2D: Failed to initialize staging buffer");
        return;
    }

    VulkanDeviceMemoryPtr stagingBufferMemory = stagingBuffer->GetDeviceMemory();
    LOG_ASSERT(stagingBufferMemory != nullptr, "VulkanTexture2D: Failed to allocate memory");

    void *data;
    LOG_TRACE("vkMapMemory %s", m_guid.c_str());
    VkResult result = vkMapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle(), 0, imageSize, 0, &data);
    if (result != VK_SUCCESS)
    {
        LOG_FATAL("VulkanTexture2D: Failed to map staging buffer memory");
        return;
    }

    // Hier wird angenommen, dass die Daten bereits im korrekten Format vorliegen
    memcpy(data, bitmapInSystemMemory, static_cast<size_t>(imageSize));
    LOG_TRACE("vkUnmapMemory %s", m_guid.c_str());
    vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle());

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseTransferCommandBuffer();

    VkImageSubresourceLayers subResource = {};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subResource.baseArrayLayer = 0;
    subResource.mipLevel = 0;
    subResource.layerCount = 1;

    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    LOG_TRACE("vkCmdCopyBufferToImage %s", m_guid.c_str());
    vkCmdCopyBufferToImage(singleTimeCommandBuffer->GetHandle(), stagingBuffer->GetHandle(), m_image, m_currentLayout, 1, &region);

    if (!singleTimeCommandBuffer->EndAndSubmit())
    {
        LOG_FATAL("VulkanTexture2D: Failed to end and submit single time command buffer");
    }

    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

std::shared_ptr<void> VulkanTexture2D::VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    FN("VulkanTexture2D::VCopyToSystemMemory");
    OPTICK_EVENT();

    LOG_ASSERT(!(format == ImageFormat::StencilIndex), "StencilIndex is not supported by CopyToBuffer, .Try DepthStencil instead");

    VerifyRowAlignment(rowAlignment);

    size_t numberOfChannels = NumberOfChannels(format);

    VkDeviceSize imageSize = m_description.GetWidth() * m_description.GetHeight() * numberOfChannels * SizeInBytes(dataType);
    VulkanBufferPtr stagingBuffer = VulkanBufferPtr(new VulkanBuffer(m_logicalDevice));

    bool success = stagingBuffer->Initialize(imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (!success)
    {
        LOG_FATAL("VulkanTexture2D: Failed to initialize staging buffer");
        return nullptr;
    }

    VulkanDeviceMemoryPtr stagingBufferMemory = stagingBuffer->GetDeviceMemory();
    LOG_ASSERT(stagingBufferMemory != nullptr, "VulkanTexture2D: Failed to allocate memory");

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseTransferCommandBuffer();

    VkImageSubresourceLayers subResource = {};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subResource.baseArrayLayer = 0;
    subResource.mipLevel = 0;
    subResource.layerCount = 1;

    VkImageLayout oldLayout = m_currentLayout;
    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {static_cast<uint32_t>(m_description.GetWidth()), static_cast<uint32_t>(m_description.GetHeight()), 1};

    LOG_TRACE("vkCmdCopyImageToBuffer %s", m_guid.c_str());
    vkCmdCopyImageToBuffer(singleTimeCommandBuffer->GetHandle(), m_image, m_currentLayout, stagingBuffer->GetHandle(), 1, &region);

    if (!singleTimeCommandBuffer->EndAndSubmit())
    {
        LOG_FATAL("VulkanTexture2D: Failed to end and submit single time command buffer");
        return nullptr;
    }

    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), oldLayout);

    void *data;
    LOG_TRACE("vkMapMemory %s", m_guid.c_str());
    VkResult result = vkMapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle(), 0, imageSize, 0, &data);
    if (result != VK_SUCCESS)
    {
        LOG_FATAL("VulkanTexture2D: Failed to map staging buffer memory");
        return nullptr;
    }

    void *bufferInSystemMemory = malloc(imageSize);
    memcpy(bufferInSystemMemory, data, static_cast<size_t>(imageSize));

    LOG_TRACE("vkUnmapMemory %s", m_guid.c_str());
    vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBufferMemory->GetHandle());

    return std::shared_ptr<void>(bufferInSystemMemory, [](void *ptr) { free(ptr); });
}

Texture2DDescription VulkanTexture2D::VGetDescription()
{
    FN("VulkanTexture2D::VGetDescription");

    return m_description;
}

VkImage VulkanTexture2D::GetHandle()
{
    FN("VulkanTexture2D::GetHandle");

    return m_image;
}

VkImageView VulkanTexture2D::GetImageView()
{
    FN("VulkanTexture2D::GetImageView");

    if (m_imageView == VK_NULL_HANDLE)
    {
        VkFormat format = VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat());
        if (!m_logicalDevice->GetParentPhysicalDevice()->IsFormatSupported(format, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
        {
            std::vector<VkFormat> fallbackFormats = {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM};
            format = m_logicalDevice->GetParentPhysicalDevice()->FindSupportedFormat(fallbackFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
            LOG_WARNING("VulkanTexture2D: Falling back to supported format: %s", VulkanTypeConverter::ToString(format).c_str());
        }

        if (format == VK_FORMAT_UNDEFINED)
        {
            LOG_ERROR("VulkanTexture2D: Unsupported texture format");
            return VK_NULL_HANDLE;
        }

        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = m_image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        if (IsDepthFormat() && IsStencilFormat())
        {
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        else if (IsDepthFormat())
        {
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else
        {
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        LOG_TRACE("vkCreateImageView %s", m_guid.c_str());
        VkResult result = vkCreateImageView(m_logicalDevice->GetHandle(), &imageViewCreateInfo, nullptr, &m_imageView);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanTexture2D: Failed to create image view. Error: %s", VulkanTypeConverter::ToString(result).c_str());
            return VK_NULL_HANDLE;
        }
    }

    return m_imageView;
}

VkImageLayout VulkanTexture2D::GetImageLayout()
{
    FN("VulkanTexture2D::GetImageLayout");

    return m_currentLayout;
}

void VulkanTexture2D::SetImageLayout(VkImageLayout imageLayout)
{
    FN("VulkanTexture2D::SetImageLayout");

    m_currentLayout = imageLayout;
}

bool VulkanTexture2D::IsStencilFormat()
{
    FN("VulkanTexture2D::IsStencilFormat");

    return IsStencilFormat(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()));
}

bool VulkanTexture2D::IsStencilFormat(VkFormat format)
{
    FN("VulkanTexture2D::IsStencilFormat");

    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

bool VulkanTexture2D::IsDepthFormat()
{
    FN("VulkanTexture2D::IsDepthFormat");

    return IsDepthFormat(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()));
}

bool VulkanTexture2D::IsDepthFormat(VkFormat format)
{
    FN("VulkanTexture2D::IsDepthFormat");

    return format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanTexture2D::TransitionImageLayout(VkImageLayout newLayout)
{
    FN("VulkanTexture2D::TransitionImageLayout");

    TransitionImageLayout(VulkanTypeConverter::ToVkFormat(m_description.GetTextureFormat()), newLayout);
}

// =========================================================================
// PRIVATE METHODS
// =========================================================================

void VulkanTexture2D::TransitionImageLayout(VkFormat format, VkImageLayout newLayout)
{
    FN("VulkanTexture2D::TransitionImageLayout");

    if (m_currentLayout == newLayout)
        return;

    VulkanSingleUseCommandBufferPtr singleTimeCommandBuffer = m_logicalDevice->CreateSingleUseTransferCommandBuffer();

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = m_currentLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = m_image;
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (IsStencilFormat(format))
        {
            imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (m_currentLayout == VK_IMAGE_LAYOUT_UNDEFINED)
    {
        imageMemoryBarrier.srcAccessMask = 0;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if (m_currentLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
    }
    else
    {
        LOG_FATAL("VulkanTexture2D: unsupported source layout transition!");
        return;
    }

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
    }
    else
    {
        LOG_FATAL("VulkanTexture2D: unsupported destination layout transition!");
        return;
    }

    vkCmdPipelineBarrier(singleTimeCommandBuffer->GetHandle(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    if (!singleTimeCommandBuffer->EndAndSubmit())
    {
        LOG_FATAL("VulkanTexture2D: Failed to end and submit single time command buffer");
    }
    m_currentLayout = newLayout;
}

} // namespace bow