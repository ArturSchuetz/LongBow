#include <VulkanRenderDevice/Device/Textures/BowVulkanTextureSampler.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanTextureSampler::VulkanTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
    : ITextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy), m_logicalDevice(nullptr), m_sampler(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID())
{
    FN("VulkanTextureSampler::VulkanTextureSampler");
}

VulkanTextureSampler::~VulkanTextureSampler()
{
    FN("VulkanTextureSampler::~VulkanTextureSampler");

    if (m_logicalDevice != nullptr)
    {
        if (m_sampler != VK_NULL_HANDLE)
        {
            LOG_TRACE("vkDestroySampler %s", m_guid.c_str());
            vkDestroySampler(m_logicalDevice->GetHandle(), m_sampler, nullptr);
        }
    }
}

bool VulkanTextureSampler::Initialize(VulkanLogicalDevice *device)
{
    FN("VulkanTextureSampler::Initialize");

    m_logicalDevice = device;

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.flags = 0;
    samplerInfo.magFilter = VulkanTypeConverter::ToVkFilter(MagnificationFilter);
    samplerInfo.minFilter = VulkanTypeConverter::ToVkFilter(MinificationFilter);
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VulkanTypeConverter::ToVkSamplerAddressMode(WrapS);
    samplerInfo.addressModeV = VulkanTypeConverter::ToVkSamplerAddressMode(WrapT);
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = MaximumAnistropy;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    LOG_TRACE("vkCreateSampler %s", m_guid.c_str());
    if (vkCreateSampler(m_logicalDevice->GetHandle(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create texture sampler!");
        return false;
    }

    return true;
}

VkSampler VulkanTextureSampler::GetHandle() const
{
    FN("VulkanTextureSampler::GetHandle");

    return m_sampler;
}

} // namespace bow