#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

namespace bow
{

class VulkanTextureSampler : public ITextureSampler
{
  public:
    VulkanTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1);
    ~VulkanTextureSampler();

    bool Initialize(VulkanLogicalDevice *device);

    VkSampler GetHandle() const;

  private:
    // you shall not copy!
    VulkanTextureSampler(const VulkanTextureSampler &) = delete;
    VulkanTextureSampler &operator=(const VulkanTextureSampler &) = delete;

    VulkanLogicalDevice *m_logicalDevice;

    VkSampler m_sampler;

    std::string m_guid;
};

} // namespace bow