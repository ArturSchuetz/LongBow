#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/BowVulkanRenderDevice.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/IBowRenderDevice.h>

namespace bow
{

class VulkanInstance
{
  public:
    VulkanInstance();
    ~VulkanInstance();

    bool Initialize();
    void Release();

    VkInstance GetHandle();

  private:
    // you shall not copy!
    VulkanInstance(const VulkanInstance &) = delete;
    VulkanInstance &operator=(const VulkanInstance &) = delete;

    static void PrintLayerProperties(const std::vector<VkLayerProperties> &layerProperties);
    static void PrintExtensionProperties(const std::vector<VkExtensionProperties> &extensionProperties);

    VkInstance m_vulkanInstance;

    std::string m_guid;
};
} // namespace bow
