#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

class VulkanPhysicalDevice
{
  public:
    VulkanPhysicalDevice();
    ~VulkanPhysicalDevice();

    bool Initialize(VkPhysicalDevice device);
    bool CheckSurfaceCapabilities(VkSurfaceKHR survace);

    VulkanLogicalDevicePtr CreateLogicalDevice(uint32_t graphicsQueueFamilyIdx, uint32_t presentQueueFamilyIdx, uint32_t computeQueueFamilyIndex, uint32_t transferQueueFamilyIndex);

    VkPhysicalDevice GetHandle() const;

    std::vector<VulkanQueueFamilyPtr> GetQueueFamilies() const;

    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const;
    VkPhysicalDeviceProperties2 GetPhysicalDeviceProperties() const;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR GetRayTracingPipelineProperties() const;
    std::vector<VkSurfaceFormatKHR> GetSurfaceFormats() const;
    std::vector<VkPresentModeKHR> GetPresentModes() const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    bool IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindSupportedDepthFormat();

  private:
    // you shall not copy!
    VulkanPhysicalDevice(const VulkanPhysicalDevice &) = delete;
    VulkanPhysicalDevice &operator=(const VulkanPhysicalDevice &) = delete;

    const void PrintDeviceProperties(VkPhysicalDeviceProperties2 pProperties) const;
    const void PrintDeviceRayTracingPipelineProperties(const VkPhysicalDeviceRayTracingPipelinePropertiesKHR &physicalDeviceRayTracingPipelineProperties) const;
    const void PrintDeviceLayerProperties(const std::vector<VkLayerProperties> &layerProperties) const;
    const void PrintExtensionProperties(const std::vector<VkExtensionProperties> &extensionProperties) const;
    const void PrintDeviceFeatures(VkPhysicalDeviceFeatures2 pFeatures) const;
    const void PrintDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties pMemoryProperties) const;
    const void PrintDeviceQueueProperties(const std::vector<VkQueueFamilyProperties> &queue_properties) const;
    const void PrintSurfaceCapabilities(VkSurfaceCapabilitiesKHR surfaceCapabilities) const;
    const void PrintSurfaceFormats(std::vector<VkSurfaceFormatKHR> surfaceFormats) const;
    const void PrintSurfacePresentationModes(std::vector<VkPresentModeKHR> surfacePresentationModes) const;

    const std::string VkFormatToString(VkFormat format) const;
    const std::string VkColorSpaceToString(VkColorSpaceKHR format) const;

    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceProperties2 m_physicalDeviceProperties;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_physicalDeviceRayTracingPipelineProperties;
    VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
    VkPhysicalDeviceFeatures2 m_physicalDeviceFeatures;
    std::vector<VulkanQueueFamilyPtr> m_queueFamilies;

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_surfacePresentationModes;

    std::string m_guid;
};
} // namespace bow
