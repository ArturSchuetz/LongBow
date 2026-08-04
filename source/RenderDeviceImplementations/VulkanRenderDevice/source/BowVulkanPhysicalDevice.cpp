#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>

#include <VulkanRenderDevice/BowVulkanQueueFamily.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanPhysicalDevice::VulkanPhysicalDevice()
    : m_physicalDevice(nullptr), m_physicalDeviceProperties(), m_physicalDeviceRayTracingPipelineProperties(), m_physicalDeviceMemoryProperties(), m_physicalDeviceFeatures(), m_queueFamilies(), m_surfaceCapabilities(), m_surfaceFormats(),
      m_surfacePresentationModes(), m_guid(Utils::GenerateGUID())
{
    FN("VulkanPhysicalDevice::VulkanPhysicalDevice");
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
    FN("VulkanPhysicalDevice::~VulkanPhysicalDevice");

    m_physicalDevice = nullptr;
}

bool VulkanPhysicalDevice::Initialize(VkPhysicalDevice physical_device)
{
    FN("VulkanPhysicalDevice::Initialize");

    m_physicalDevice = physical_device;

    // Get properties of the physical device.
    LOG_TRACE("vkGetPhysicalDeviceProperties %s", m_guid.c_str());

    m_physicalDeviceRayTracingPipelineProperties = {};
    m_physicalDeviceRayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    m_physicalDeviceProperties = {};
    m_physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    m_physicalDeviceProperties.pNext = &m_physicalDeviceRayTracingPipelineProperties;

    vkGetPhysicalDeviceProperties2(m_physicalDevice, &m_physicalDeviceProperties);

    PrintDeviceProperties(m_physicalDeviceProperties);                                     // Log device properties.
    PrintDeviceRayTracingPipelineProperties(m_physicalDeviceRayTracingPipelineProperties); // Log ray tracing device properties.

    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
    accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    m_physicalDeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    m_physicalDeviceFeatures.pNext = &accelerationStructureFeatures;

    // Get device features.
    LOG_TRACE("vkGetPhysicalDeviceFeatures %s", m_guid.c_str());
    vkGetPhysicalDeviceFeatures2(m_physicalDevice, &m_physicalDeviceFeatures);
    PrintDeviceFeatures(m_physicalDeviceFeatures); // Log device features.

    // Get device memory properties.
    LOG_TRACE("vkGetPhysicalDeviceMemoryProperties %s", m_guid.c_str());
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemoryProperties);
    PrintDeviceMemoryProperties(m_physicalDeviceMemoryProperties); // Log memory properties.

    // ##################### Queues #################################

    // Get queue family properties.
    uint32_t queue_family_count = 1;
    LOG_TRACE("vkGetPhysicalDeviceQueueFamilyProperties %s", m_guid.c_str());
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queue_family_count, nullptr);
    if (queue_family_count == 0)
    {
        LOG_ERROR("VulkanPhysicalDevice: No Supported Queues");
        return false; // No queue families found.
    }

    std::vector<VkQueueFamilyProperties> m_queue_properties(queue_family_count);
    LOG_TRACE("vkGetPhysicalDeviceQueueFamilyProperties %s", m_guid.c_str());
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queue_family_count, m_queue_properties.data());
    PrintDeviceQueueProperties(m_queue_properties); // Log queue family properties.

    for (int i = 0; i < m_queue_properties.size(); i++)
    {
        VulkanQueueFamilyPtr queueFamily = VulkanQueueFamilyPtr(new VulkanQueueFamily(i, m_queue_properties[i]));
        m_queueFamilies.push_back(queueFamily);
    }

    // ##################### Layer Properties #################################

    // Check for supported layers on the device.
    uint32_t pPropertyCount = 0;
    LOG_TRACE("vkEnumerateDeviceLayerProperties %s", m_guid.c_str());
    VkResult layer_properties_result = vkEnumerateDeviceLayerProperties(m_physicalDevice, &pPropertyCount, nullptr);
    if (layer_properties_result != VK_SUCCESS || pPropertyCount == 0)
    {
        if (pPropertyCount == 0)
            LOG_ERROR("VulkanPhysicalDevice: No Supported Layers");
        else
            LOG_ERROR("VulkanPhysicalDevice: %s", VulkanTypeConverter::ToString(layer_properties_result).c_str());
        return false; // No layers found or error.
    }

    std::vector<VkLayerProperties> layerProperties(pPropertyCount);
    LOG_TRACE("vkEnumerateDeviceLayerProperties %s", m_guid.c_str());
    layer_properties_result = vkEnumerateDeviceLayerProperties(m_physicalDevice, &pPropertyCount, &layerProperties[0]);
    if (layer_properties_result != VK_SUCCESS || pPropertyCount == 0)
    {
        if (pPropertyCount == 0)
            LOG_ERROR("VulkanPhysicalDevice: No Supported Layers");
        else
            LOG_ERROR("VulkanPhysicalDevice: %s", VulkanTypeConverter::ToString(layer_properties_result).c_str());
        return false; // No layers found or error.
    }

    PrintDeviceLayerProperties(layerProperties); // Log layer properties.

    // ##################### Extension Properties
    // #################################

    // Check for supported extensions on the device.
    uint32_t pExtensionsCount = 0;
    LOG_TRACE("vkEnumerateDeviceExtensionProperties %s", m_guid.c_str());
    VkResult extension_enum_result = vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &pExtensionsCount, nullptr);
    if (extension_enum_result != VK_SUCCESS || pExtensionsCount == 0)
    {
        if (pExtensionsCount == 0)
            LOG_ERROR("VulkanPhysicalDevice: No Supported Extensions");
        else
            LOG_ERROR("VulkanPhysicalDevice: %s", VulkanTypeConverter::ToString(layer_properties_result).c_str());
        return false; // No extensions found or error.
    }

    std::vector<VkExtensionProperties> extensionProperties(pExtensionsCount);
    LOG_TRACE("vkEnumerateDeviceExtensionProperties %s", m_guid.c_str());
    extension_enum_result = vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &pExtensionsCount, &extensionProperties[0]);
    if (extension_enum_result != VK_SUCCESS || pExtensionsCount == 0)
    {
        if (pExtensionsCount == 0)
            LOG_ERROR("VulkanPhysicalDevice: No Supported Extensions");
        else
            LOG_ERROR("VulkanPhysicalDevice: %s", VulkanTypeConverter::ToString(layer_properties_result).c_str());
        return false; // No extensions found or error.
    }
    PrintExtensionProperties(extensionProperties); // Log extension properties.

    LOG_INFO("VulkanPhysicalDevice initialized");
    return true;
}

bool VulkanPhysicalDevice::CheckSurfaceCapabilities(VkSurfaceKHR surface)
{
    FN("VulkanPhysicalDevice::CheckSurfaceCapabilities");

    // ##################### Surface Capabilities KHR
    // #################################

    // Get the physical device's surface capabilities
    LOG_TRACE("vkGetPhysicalDeviceSurfaceCapabilitiesKHR %s", m_guid.c_str());
    VkResult surfaceCapabilitiesKHR_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &m_surfaceCapabilities);
    if (surfaceCapabilitiesKHR_result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanPhysicalDevice: %s", VulkanTypeConverter::ToString(surfaceCapabilitiesKHR_result).c_str());
        return false; // No extensions found or error.
    }

    PrintSurfaceCapabilities(m_surfaceCapabilities);

    // ##################### Surface Formats KHR
    // #################################

    // Get the supported surface formats
    uint32_t surface_formats_count;
    LOG_TRACE("vkGetPhysicalDeviceSurfaceFormatsKHR %s", m_guid.c_str());
    VkResult surfaceFormatsKHR_result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &surface_formats_count, nullptr);
    if (surfaceFormatsKHR_result != VK_SUCCESS || surface_formats_count == 0)
    {
        LOG_ERROR("VulkanPhysicalDevice: could not fetch surface formats: %s", VulkanTypeConverter::ToString(surfaceCapabilitiesKHR_result).c_str());
        return false;
    }

    m_surfaceFormats.resize(surface_formats_count);
    LOG_TRACE("vkGetPhysicalDeviceSurfaceFormatsKHR %s", m_guid.c_str());
    surfaceFormatsKHR_result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &surface_formats_count, &m_surfaceFormats[0]);
    if (surfaceFormatsKHR_result != VK_SUCCESS || surface_formats_count == 0)
    {
        LOG_ERROR("VulkanPhysicalDevice: could not fetch surface formats: %s", VulkanTypeConverter::ToString(surfaceCapabilitiesKHR_result).c_str());
        return false;
    }
    PrintSurfaceFormats(m_surfaceFormats);

    // ##################### Surface Present Modes KHR
    // #################################

    // Get the supported presentation modes
    uint32_t surface_presentation_modes_count;
    LOG_TRACE("vkGetPhysicalDeviceSurfacePresentModesKHR %s", m_guid.c_str());
    VkResult surfacePresentModesKHR_result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &surface_presentation_modes_count, nullptr);
    if (surfacePresentModesKHR_result != VK_SUCCESS || surface_presentation_modes_count == 0)
    {
        LOG_ERROR("VulkanPhysicalDevice: could not fetch surface presentation modes: %s", VulkanTypeConverter::ToString(surfaceCapabilitiesKHR_result).c_str());
        return false;
    }

    m_surfacePresentationModes.resize(surface_presentation_modes_count);
    LOG_TRACE("vkGetPhysicalDeviceSurfacePresentModesKHR %s", m_guid.c_str());
    surfacePresentModesKHR_result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &surface_presentation_modes_count, &m_surfacePresentationModes[0]);
    if (surfacePresentModesKHR_result != VK_SUCCESS || surface_presentation_modes_count == 0)
    {
        LOG_ERROR("VulkanPhysicalDevice: could not fetch surface presentation modes: %s", VulkanTypeConverter::ToString(surfaceCapabilitiesKHR_result).c_str());
        return false;
    }
    PrintSurfacePresentationModes(m_surfacePresentationModes);
}

VulkanLogicalDevicePtr VulkanPhysicalDevice::CreateLogicalDevice(uint32_t graphicsQueueFamilyIdx, uint32_t presentQueueFamilyIdx, uint32_t computeQueueFamilyIndex, uint32_t transferQueueFamilyIndex)
{
    FN("VulkanPhysicalDevice::CreateLogicalDevice");

    VulkanLogicalDevicePtr logicalDevice = VulkanLogicalDevicePtr(new VulkanLogicalDevice());
    if (!logicalDevice->Initialize(this, graphicsQueueFamilyIdx, presentQueueFamilyIdx, computeQueueFamilyIndex, transferQueueFamilyIndex))
    {
        LOG_ERROR("VulkanPhysicalDevice: Failed to create logical device");
        return nullptr;
    }
    return logicalDevice;
}

VkPhysicalDevice VulkanPhysicalDevice::GetHandle() const
{
    FN("VulkanPhysicalDevice::GetHandle");

    return m_physicalDevice;
}

std::vector<VulkanQueueFamilyPtr> VulkanPhysicalDevice::GetQueueFamilies() const
{
    FN("VulkanPhysicalDevice::GetQueueFamilies");

    return m_queueFamilies;
}

VkSurfaceCapabilitiesKHR VulkanPhysicalDevice::GetSurfaceCapabilities() const
{
    FN("VulkanPhysicalDevice::GetSurfaceCapabilities");

    return m_surfaceCapabilities;
}

VkPhysicalDeviceProperties2 VulkanPhysicalDevice::GetPhysicalDeviceProperties() const
{
    FN("VulkanPhysicalDevice::GetPhysicalDeviceProperties");

    return m_physicalDeviceProperties;
}

VkPhysicalDeviceRayTracingPipelinePropertiesKHR VulkanPhysicalDevice::GetRayTracingPipelineProperties() const
{
    FN("VulkanPhysicalDevice::GetRayTracingPipelineProperties");

    return m_physicalDeviceRayTracingPipelineProperties;
}

std::vector<VkSurfaceFormatKHR> VulkanPhysicalDevice::GetSurfaceFormats() const
{
    FN("VulkanPhysicalDevice::GetSurfaceFormats");

    return m_surfaceFormats;
}

std::vector<VkPresentModeKHR> VulkanPhysicalDevice::GetPresentModes() const
{
    FN("VulkanPhysicalDevice::GetPresentModes");

    return m_surfacePresentationModes;
}

uint32_t VulkanPhysicalDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    FN("VulkanPhysicalDevice::FindMemoryType");

    for (uint32_t i = 0; i < m_physicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (m_physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    LOG_ERROR("VulkanPhysicalDevice: Failed to find suitable memory type");
    return 0;
}

bool VulkanPhysicalDevice::IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    FN("VulkanPhysicalDevice::IsFormatSupported");

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProperties);

    if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features) == features)
    {
        return true;
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & features) == features)
    {
        return true;
    }

    return false;
}

VkFormat VulkanPhysicalDevice::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    FN("VulkanPhysicalDevice::FindSupportedFormat");

    for (VkFormat format : candidates)
    {
        if (IsFormatSupported(format, tiling, features))
        {
            return format;
        }
    }
    LOG_FATAL("VulkanTexture2D: Failed to find supported format");
}

VkFormat VulkanPhysicalDevice::FindSupportedDepthFormat()
{
    FN("VulkanPhysicalDevice::FindSupportedDepthFormat");

    std::vector<VkFormat> possibleFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT};
    return FindSupportedFormat(possibleFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

// =========================================================================
// PRIVATE METHODS
// =========================================================================

const void VulkanPhysicalDevice::PrintDeviceProperties(VkPhysicalDeviceProperties2 pProperties) const
{
    FN("VulkanPhysicalDevice::PrintDeviceProperties");

    LOG_TRACE("Physical Device: %s", pProperties.properties.deviceName);
    LOG_TRACE("\tapiVersion: %u", pProperties.properties.apiVersion);
    LOG_TRACE("\tdriverVersion: %u", pProperties.properties.driverVersion);
    LOG_TRACE("\tvendorID: %u", pProperties.properties.vendorID);
    LOG_TRACE("\tdeviceID: %u", pProperties.properties.deviceID);

    switch (pProperties.properties.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        LOG_TRACE("\tdeviceType: VK_PHYSICAL_DEVICE_TYPE_OTHER");
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        LOG_TRACE("\tdeviceType: VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU");
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        LOG_TRACE("\tdeviceType: VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU");
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        LOG_TRACE("\tdeviceType: VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU");
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        LOG_TRACE("\tdeviceType: VK_PHYSICAL_DEVICE_TYPE_CPU");
        break;
    }

    std::string chacheuuid;
    for (size_t j = 0; j < VK_UUID_SIZE; j++)
    {
        chacheuuid.append(std::to_string((int)pProperties.properties.pipelineCacheUUID[j]));
    }

    LOG_TRACE("\tchacheuuid: %s", chacheuuid.c_str());

    LOG_TRACE("LIMITS:");
    LOG_TRACE("\tmaxImageDimension1D: %u", pProperties.properties.limits.maxImageDimension1D);
    LOG_TRACE("\tmaxImageDimension2D: %u", pProperties.properties.limits.maxImageDimension2D);
    LOG_TRACE("\tmaxImageDimension3D: %u", pProperties.properties.limits.maxImageDimension3D);
    LOG_TRACE("\tmaxImageDimensionCube: %u", pProperties.properties.limits.maxImageDimensionCube);
    LOG_TRACE("\tmaxImageArrayLayers: %u", pProperties.properties.limits.maxImageArrayLayers);
    LOG_TRACE("\tmaxTexelBufferElements: %u", pProperties.properties.limits.maxTexelBufferElements);
    LOG_TRACE("\tmaxUniformBufferRange: %u", pProperties.properties.limits.maxUniformBufferRange);
    LOG_TRACE("\tmaxStorageBufferRange: %u", pProperties.properties.limits.maxStorageBufferRange);
    LOG_TRACE("\tmaxPushConstantsSize: %u", pProperties.properties.limits.maxPushConstantsSize);
    LOG_TRACE("\tmaxMemoryAllocationCount: %u", pProperties.properties.limits.maxMemoryAllocationCount);
    LOG_TRACE("\tmaxSamplerAllocationCount: %u", pProperties.properties.limits.maxSamplerAllocationCount);
    LOG_TRACE("\tbufferImageGranularity: %u", pProperties.properties.limits.bufferImageGranularity);
    LOG_TRACE("\tsparseAddressSpaceSize: %u", pProperties.properties.limits.sparseAddressSpaceSize);
    LOG_TRACE("\tmaxBoundDescriptorSets: %u", pProperties.properties.limits.maxBoundDescriptorSets);
    LOG_TRACE("\tmaxPerStageDescriptorSamplers: %u", pProperties.properties.limits.maxPerStageDescriptorSamplers);
    LOG_TRACE("\tmaxPerStageDescriptorUniformBuffers: %u", pProperties.properties.limits.maxPerStageDescriptorUniformBuffers);
    LOG_TRACE("\tmaxPerStageDescriptorStorageBuffers: %u", pProperties.properties.limits.maxPerStageDescriptorStorageBuffers);
    LOG_TRACE("\tmaxPerStageDescriptorSampledImages: %u", pProperties.properties.limits.maxPerStageDescriptorSampledImages);
    LOG_TRACE("\tmaxPerStageDescriptorStorageImages: %u", pProperties.properties.limits.maxPerStageDescriptorStorageImages);
    LOG_TRACE("\tmaxPerStageDescriptorInputAttachments: %u", pProperties.properties.limits.maxPerStageDescriptorInputAttachments);
    LOG_TRACE("\tmaxPerStageResources: %u", pProperties.properties.limits.maxPerStageResources);
    LOG_TRACE("\tmaxDescriptorSetSamplers: %u", pProperties.properties.limits.maxDescriptorSetSamplers);
    LOG_TRACE("\tmaxDescriptorSetUniformBuffers %u", pProperties.properties.limits.maxDescriptorSetUniformBuffers);
    LOG_TRACE("\tmaxDescriptorSetUniformBuffersDynamic: %u", pProperties.properties.limits.maxDescriptorSetUniformBuffersDynamic);
    LOG_TRACE("\tmaxDescriptorSetStorageBuffers: %u", pProperties.properties.limits.maxDescriptorSetStorageBuffers);
    LOG_TRACE("\tmaxDescriptorSetStorageBuffersDynamic: %u", pProperties.properties.limits.maxDescriptorSetStorageBuffersDynamic);
    LOG_TRACE("\tmaxDescriptorSetSampledImages: %u", pProperties.properties.limits.maxDescriptorSetSampledImages);
    LOG_TRACE("\tmaxDescriptorSetStorageImages: %u", pProperties.properties.limits.maxDescriptorSetStorageImages);
    LOG_TRACE("\tmaxDescriptorSetInputAttachments: %u", pProperties.properties.limits.maxDescriptorSetInputAttachments);
    LOG_TRACE("\tmaxVertexInputAttributes: %u", pProperties.properties.limits.maxVertexInputAttributes);
    LOG_TRACE("\tmaxVertexInputBindings: %u", pProperties.properties.limits.maxVertexInputBindings);
    LOG_TRACE("\tmaxVertexInputAttributeOffset: %u", pProperties.properties.limits.maxVertexInputAttributeOffset);
    LOG_TRACE("\tmaxVertexInputBindingStride: %u", pProperties.properties.limits.maxVertexInputBindingStride);
    LOG_TRACE("\tmaxVertexOutputComponents: %u", pProperties.properties.limits.maxVertexOutputComponents);
    LOG_TRACE("\tmaxTessellationGenerationLevel: %u", pProperties.properties.limits.maxTessellationGenerationLevel);
    LOG_TRACE("\tmaxTessellationPatchSize: %u", pProperties.properties.limits.maxTessellationPatchSize);
    LOG_TRACE("\tmaxTessellationControlPerVertexInputComponents: %u", pProperties.properties.limits.maxTessellationControlPerVertexInputComponents);
    LOG_TRACE("\tmaxTessellationControlPerVertexOutputComponents: %u", pProperties.properties.limits.maxTessellationControlPerVertexOutputComponents);
    LOG_TRACE("\tmaxTessellationControlPerPatchOutputComponents: %u", pProperties.properties.limits.maxTessellationControlPerPatchOutputComponents);
    LOG_TRACE("\tmaxTessellationControlTotalOutputComponents: %u", pProperties.properties.limits.maxTessellationControlTotalOutputComponents);
    LOG_TRACE("\tmaxTessellationEvaluationInputComponents: %u", pProperties.properties.limits.maxTessellationEvaluationInputComponents);
    LOG_TRACE("\tmaxTessellationEvaluationOutputComponents: %u", pProperties.properties.limits.maxTessellationEvaluationOutputComponents);
    LOG_TRACE("\tmaxGeometryShaderInvocations: %u", pProperties.properties.limits.maxGeometryShaderInvocations);
    LOG_TRACE("\tmaxGeometryInputComponents: %u", pProperties.properties.limits.maxGeometryInputComponents);
    LOG_TRACE("\tmaxGeometryOutputComponents: %u", pProperties.properties.limits.maxGeometryOutputComponents);
    LOG_TRACE("\tmaxGeometryOutputVertices %u", pProperties.properties.limits.maxGeometryOutputVertices);
    LOG_TRACE("\tmaxGeometryTotalOutputComponents: %u", pProperties.properties.limits.maxGeometryTotalOutputComponents);
    LOG_TRACE("\tmaxFragmentInputComponents: %u", pProperties.properties.limits.maxFragmentInputComponents);
    LOG_TRACE("\tmaxFragmentOutputAttachments: %u", pProperties.properties.limits.maxFragmentOutputAttachments);
    LOG_TRACE("\tmaxFragmentDualSrcAttachments: %u", pProperties.properties.limits.maxFragmentDualSrcAttachments);
    LOG_TRACE("\tmaxFragmentCombinedOutputResources: %u", pProperties.properties.limits.maxFragmentCombinedOutputResources);
    LOG_TRACE("\tmaxComputeSharedMemorySize: %u", pProperties.properties.limits.maxComputeSharedMemorySize);
    LOG_TRACE("\tmaxComputeWorkGroupCount: %u %u %u", pProperties.properties.limits.maxComputeWorkGroupCount[0], pProperties.properties.limits.maxComputeWorkGroupCount[1], pProperties.properties.limits.maxComputeWorkGroupCount[2]);
    LOG_TRACE("\tmaxComputeWorkGroupInvocations: %u", pProperties.properties.limits.maxComputeWorkGroupInvocations);
    LOG_TRACE("\tmaxComputeWorkGroupSize: %u %u %u", pProperties.properties.limits.maxComputeWorkGroupSize[0], pProperties.properties.limits.maxComputeWorkGroupSize[1], pProperties.properties.limits.maxComputeWorkGroupSize[2]);
    LOG_TRACE("\tsubPixelPrecisionBits: %u", pProperties.properties.limits.subPixelPrecisionBits);
    LOG_TRACE("\tsubTexelPrecisionBits: %u", pProperties.properties.limits.subTexelPrecisionBits);
    LOG_TRACE("\tmipmapPrecisionBits: %u", pProperties.properties.limits.mipmapPrecisionBits);
    LOG_TRACE("\tmaxDrawIndexedIndexValue: %u", pProperties.properties.limits.maxDrawIndexedIndexValue);
    LOG_TRACE("\tmaxDrawIndirectCount: %u", pProperties.properties.limits.maxDrawIndirectCount);
    LOG_TRACE("\tmaxSamplerLodBias: %f", pProperties.properties.limits.maxSamplerLodBias);
    LOG_TRACE("\tmaxSamplerAnisotropy: %f", pProperties.properties.limits.maxSamplerAnisotropy);
    LOG_TRACE("\tmaxViewports: %u", pProperties.properties.limits.maxViewports);
    LOG_TRACE("\tmaxViewportDimensions: %u %u", pProperties.properties.limits.maxViewportDimensions[0], pProperties.properties.limits.maxViewportDimensions[1]);
    LOG_TRACE("\tviewportBoundsRange: %f %f", pProperties.properties.limits.viewportBoundsRange[0], pProperties.properties.limits.viewportBoundsRange[1]);
    LOG_TRACE("\tviewportSubPixelBits: %u", pProperties.properties.limits.viewportSubPixelBits);
    LOG_TRACE("\tminMemoryMapAlignment: %u", pProperties.properties.limits.minMemoryMapAlignment);
    LOG_TRACE("\tminTexelBufferOffsetAlignment: %u", pProperties.properties.limits.minTexelBufferOffsetAlignment);
    LOG_TRACE("\tminUniformBufferOffsetAlignment: %u", pProperties.properties.limits.minUniformBufferOffsetAlignment);
    LOG_TRACE("\tminStorageBufferOffsetAlignment: %u", pProperties.properties.limits.minStorageBufferOffsetAlignment);
    LOG_TRACE("\tminTexelOffset: %u", pProperties.properties.limits.minTexelOffset);
    LOG_TRACE("\tmaxTexelOffset: %u", pProperties.properties.limits.maxTexelOffset);
    LOG_TRACE("\tminTexelGatherOffset: %u", pProperties.properties.limits.minTexelGatherOffset);
    LOG_TRACE("\tmaxTexelGatherOffset: %u", pProperties.properties.limits.maxTexelGatherOffset);
    LOG_TRACE("\tminInterpolationOffset %f", pProperties.properties.limits.minInterpolationOffset);
    LOG_TRACE("\tmaxInterpolationOffset: %f", pProperties.properties.limits.maxInterpolationOffset);
    LOG_TRACE("\tsubPixelInterpolationOffsetBits: %u", pProperties.properties.limits.subPixelInterpolationOffsetBits);
    LOG_TRACE("\tmaxFramebufferWidth: %u", pProperties.properties.limits.maxFramebufferWidth);
    LOG_TRACE("\tmaxFramebufferHeight: %u", pProperties.properties.limits.maxFramebufferHeight);
    LOG_TRACE("\tmaxFramebufferLayers: %u", pProperties.properties.limits.maxFramebufferLayers);

    std::string framebufferColorSampleCountsString = "";
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        framebufferColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tframebufferColorSampleCounts: %s", framebufferColorSampleCountsString.c_str());

    std::string framebufferDepthSampleCountsString = "";
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        framebufferDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tframebufferDepthSampleCounts: %s", framebufferDepthSampleCountsString.c_str());

    std::string framebufferStencilSampleCountsString = "";
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        framebufferStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tframebufferStencilSampleCounts: %s", framebufferStencilSampleCountsString.c_str());

    std::string framebufferNoAttachmentsSampleCountsString = "";
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        framebufferNoAttachmentsSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tframebufferNoAttachmentsSampleCounts: %s", framebufferNoAttachmentsSampleCountsString.c_str());

    LOG_TRACE("\tmaxColorAttachments: %u", pProperties.properties.limits.maxColorAttachments);

    std::string sampledImageColorSampleCountsString = "";
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        sampledImageColorSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tsampledImageColorSampleCounts: %s", sampledImageColorSampleCountsString.c_str());

    std::string sampledImageIntegerSampleCountsString = "";
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        sampledImageIntegerSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tsampledImageIntegerSampleCounts: %s", sampledImageIntegerSampleCountsString.c_str());

    std::string sampledImageDepthSampleCountsString = "";
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        sampledImageDepthSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tsampledImageDepthSampleCounts: %s", sampledImageDepthSampleCountsString.c_str());

    std::string sampledImageStencilSampleCountsString = "";
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_1_BIT");
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_2_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_2_BIT");
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_4_BIT");
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_8_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_8_BIT");
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_16_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_16_BIT");
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_32_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_32_BIT");
    if (pProperties.properties.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_64_BIT)
        sampledImageStencilSampleCountsString.append("\n\t\tVK_SAMPLE_COUNT_64_BIT");
    LOG_TRACE("\tsampledImageStencilSampleCounts: %s", sampledImageStencilSampleCountsString.c_str());

    LOG_TRACE("\tstorageImageSampleCounts: %u", pProperties.properties.limits.storageImageSampleCounts);
    LOG_TRACE("\tmaxSampleMaskWords: %u", pProperties.properties.limits.maxSampleMaskWords);
    LOG_TRACE("\timestampComputeAndGraphics: %s", (pProperties.properties.limits.timestampComputeAndGraphics ? "true" : "false"));
    LOG_TRACE("\timestampPeriod: %f", pProperties.properties.limits.timestampPeriod);
    LOG_TRACE("\tmaxClipDistances: %u", pProperties.properties.limits.maxClipDistances);
    LOG_TRACE("\tmaxCullDistances: %u", pProperties.properties.limits.maxCullDistances);
    LOG_TRACE("\tmaxCombinedClipAndCullDistances: %u", pProperties.properties.limits.maxCombinedClipAndCullDistances);
    LOG_TRACE("\tdiscreteQueuePriorities: %u", pProperties.properties.limits.discreteQueuePriorities);
    LOG_TRACE("\tpointSizeRange: %f %f", pProperties.properties.limits.pointSizeRange[0], pProperties.properties.limits.pointSizeRange[1]);
    LOG_TRACE("\tlineWidthRange: %f %f", pProperties.properties.limits.lineWidthRange[0], pProperties.properties.limits.lineWidthRange[1]);
    LOG_TRACE("\tpointSizeGranularity: %f", pProperties.properties.limits.pointSizeGranularity);
    LOG_TRACE("\tlineWidthGranularity: %f", pProperties.properties.limits.lineWidthGranularity);
    LOG_TRACE("\tstrictLines: %s", (pProperties.properties.limits.strictLines ? "true" : "false"));
    LOG_TRACE("\tstandardSampleLocations: %s", (pProperties.properties.limits.standardSampleLocations ? "true" : "false"));
    LOG_TRACE("\toptimalBufferCopyOffsetAlignment: %u", pProperties.properties.limits.optimalBufferCopyOffsetAlignment);
    LOG_TRACE("\toptimalBufferCopyRowPitchAlignment: %u", pProperties.properties.limits.optimalBufferCopyRowPitchAlignment);
    LOG_TRACE("\tnonCoherentAtomSize: %u", pProperties.properties.limits.nonCoherentAtomSize);
    LOG_TRACE("SPARSE PROPERTIES:");
    LOG_TRACE("\tresidencyStandard2DBlockShape: %s", (pProperties.properties.sparseProperties.residencyStandard2DBlockShape ? "true" : "false"));
    LOG_TRACE("\tresidencyStandard2DMultisampleBlockShape: %s", (pProperties.properties.sparseProperties.residencyStandard2DMultisampleBlockShape ? "true" : "false"));
    LOG_TRACE("\tresidencyStandard3DBlockShape: %s", (pProperties.properties.sparseProperties.residencyStandard3DBlockShape ? "true" : "false"));
    LOG_TRACE("\tresidencyAlignedMipSize: %s", (pProperties.properties.sparseProperties.residencyAlignedMipSize ? "true" : "false"));
    LOG_TRACE("\tresidencyNonResidentStrict: %s", (pProperties.properties.sparseProperties.residencyNonResidentStrict ? "true" : "false"));
}

const void VulkanPhysicalDevice::PrintDeviceRayTracingPipelineProperties(const VkPhysicalDeviceRayTracingPipelinePropertiesKHR &physicalDeviceRayTracingPipelineProperties) const
{
    FN("VulkanPhysicalDevice::PrintDeviceRayTracingPipelineProperties");

    LOG_TRACE("RAY TRACING PIPELINE PROPERTIES:");
    LOG_TRACE("\tshaderGroupHandleSize: %u", physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize);
    LOG_TRACE("\tmaxRayRecursionDepth: %u", physicalDeviceRayTracingPipelineProperties.maxRayRecursionDepth);
    LOG_TRACE("\tmaxShaderGroupStride: %u", physicalDeviceRayTracingPipelineProperties.maxShaderGroupStride);
    LOG_TRACE("\tshaderGroupBaseAlignment: %u", physicalDeviceRayTracingPipelineProperties.shaderGroupBaseAlignment);
    LOG_TRACE("\tshaderGroupHandleCaptureReplaySize: %u", physicalDeviceRayTracingPipelineProperties.shaderGroupHandleCaptureReplaySize);
    LOG_TRACE("\tmaxRayDispatchInvocationCount: %u", physicalDeviceRayTracingPipelineProperties.maxRayDispatchInvocationCount);
    LOG_TRACE("\tshaderGroupHandleAlignment: %u", physicalDeviceRayTracingPipelineProperties.shaderGroupHandleAlignment);
    LOG_TRACE("\tmaxRayHitAttributeSize: %u", physicalDeviceRayTracingPipelineProperties.maxRayHitAttributeSize);
}

const void VulkanPhysicalDevice::PrintDeviceLayerProperties(const std::vector<VkLayerProperties> &layerProperties) const
{
    FN("VulkanPhysicalDevice::PrintDeviceLayerProperties");

    LOG_TRACE("DEVICE LAYERS:");
    for (size_t l = 0; l < layerProperties.size(); l++)
    {
        LOG_TRACE("\tLayerName: %s", layerProperties[l].layerName);
        LOG_TRACE("\t\tspecVersion: %u", layerProperties[l].specVersion);
        LOG_TRACE("\t\timplementationVersion: %u", layerProperties[l].implementationVersion);
        LOG_TRACE("\t\tdescription: %s", layerProperties[l].description);
    }
}

const void VulkanPhysicalDevice::PrintExtensionProperties(const std::vector<VkExtensionProperties> &extensionProperties) const
{
    FN("VulkanPhysicalDevice::PrintExtensionProperties");

    LOG_TRACE("DEVICE EXTENSIONS:");
    for (size_t l = 0; l < extensionProperties.size(); l++)
    {
        LOG_TRACE("\tExtensionName: %s", extensionProperties[l].extensionName);
        LOG_TRACE("\t\tspecVersion: %u", extensionProperties[l].specVersion);
    }
}

const void VulkanPhysicalDevice::PrintDeviceFeatures(VkPhysicalDeviceFeatures2 pFeatures) const
{
    FN("VulkanPhysicalDevice::PrintDeviceFeatures");

    LOG_TRACE("FEATURES:");
    LOG_TRACE("\trobustBufferAccess: %s", (pFeatures.features.robustBufferAccess ? "true" : "false"));
    LOG_TRACE("\tfullDrawIndexUint32: %s", (pFeatures.features.fullDrawIndexUint32 ? "true" : "false"));
    LOG_TRACE("\timageCubeArray: %s", (pFeatures.features.imageCubeArray ? "true" : "false"));
    LOG_TRACE("\tindependentBlend: %s", (pFeatures.features.independentBlend ? "true" : "false"));
    LOG_TRACE("\tgeometryShader: %s", (pFeatures.features.geometryShader ? "true" : "false"));
    LOG_TRACE("\ttessellationShader: %s", (pFeatures.features.tessellationShader ? "true" : "false"));
    LOG_TRACE("\tsampleRateShading: %s", (pFeatures.features.sampleRateShading ? "true" : "false"));
    LOG_TRACE("\tdualSrcBlend: %s", (pFeatures.features.dualSrcBlend ? "true" : "false"));
    LOG_TRACE("\tlogicOp: %s", (pFeatures.features.logicOp ? "true" : "false"));
    LOG_TRACE("\tmultiDrawIndirect: %s", (pFeatures.features.multiDrawIndirect ? "true" : "false"));
    LOG_TRACE("\tdrawIndirectFirstInstance: %s", (pFeatures.features.drawIndirectFirstInstance ? "true" : "false"));
    LOG_TRACE("\tdepthClamp: %s", (pFeatures.features.depthClamp ? "true" : "false"));
    LOG_TRACE("\tdepthBiasClamp: %s", (pFeatures.features.depthBiasClamp ? "true" : "false"));
    LOG_TRACE("\tfillModeNonSolid: %s", (pFeatures.features.fillModeNonSolid ? "true" : "false"));
    LOG_TRACE("\tdepthBounds: %s", (pFeatures.features.depthBounds ? "true" : "false"));
    LOG_TRACE("\twideLines: %s", (pFeatures.features.wideLines ? "true" : "false"));
    LOG_TRACE("\tlargePoints: %s", (pFeatures.features.largePoints ? "true" : "false"));
    LOG_TRACE("\talphaToOne: %s", (pFeatures.features.alphaToOne ? "true" : "false"));
    LOG_TRACE("\tmultiViewport: %s", (pFeatures.features.multiViewport ? "true" : "false"));
    LOG_TRACE("\tsamplerAnisotropy: %s", (pFeatures.features.samplerAnisotropy ? "true" : "false"));
    LOG_TRACE("\ttextureCompressionETC2: %s", (pFeatures.features.textureCompressionETC2 ? "true" : "false"));
    LOG_TRACE("\ttextureCompressionASTC_LDR: %s", (pFeatures.features.textureCompressionASTC_LDR ? "true" : "false"));
    LOG_TRACE("\ttextureCompressionBC: %s", (pFeatures.features.textureCompressionBC ? "true" : "false"));
    LOG_TRACE("\tocclusionQueryPrecise: %s", (pFeatures.features.occlusionQueryPrecise ? "true" : "false"));
    LOG_TRACE("\tpipelineStatisticsQuery: %s", (pFeatures.features.pipelineStatisticsQuery ? "true" : "false"));
    LOG_TRACE("\tvertexPipelineStoresAndAtomics: %s", (pFeatures.features.vertexPipelineStoresAndAtomics ? "true" : "false"));
    LOG_TRACE("\tfragmentStoresAndAtomics: %s", (pFeatures.features.fragmentStoresAndAtomics ? "true" : "false"));
    LOG_TRACE("\tshaderTessellationAndGeometryPointSize: %s", (pFeatures.features.shaderTessellationAndGeometryPointSize ? "true" : "false"));
    LOG_TRACE("\tshaderImageGatherExtended: %s", (pFeatures.features.shaderImageGatherExtended ? "true" : "false"));
    LOG_TRACE("\tshaderStorageImageExtendedFormats: %s", (pFeatures.features.shaderStorageImageExtendedFormats ? "true" : "false"));
    LOG_TRACE("\tshaderStorageImageMultisample: %s", (pFeatures.features.shaderStorageImageMultisample ? "true" : "false"));
    LOG_TRACE("\tshaderStorageImageReadWithoutFormat: %s", (pFeatures.features.shaderStorageImageReadWithoutFormat ? "true" : "false"));
    LOG_TRACE("\tshaderStorageImageWriteWithoutFormat: %s", (pFeatures.features.shaderStorageImageWriteWithoutFormat ? "true" : "false"));
    LOG_TRACE("\tshaderUniformBufferArrayDynamicIndexing: %s", (pFeatures.features.shaderUniformBufferArrayDynamicIndexing ? "true" : "false"));
    LOG_TRACE("\tshaderSampledImageArrayDynamicIndexing: %s", (pFeatures.features.shaderSampledImageArrayDynamicIndexing ? "true" : "false"));
    LOG_TRACE("\tshaderStorageBufferArrayDynamicIndexing: %s", (pFeatures.features.shaderStorageBufferArrayDynamicIndexing ? "true" : "false"));
    LOG_TRACE("\tshaderStorageImageArrayDynamicIndexing: %s", (pFeatures.features.shaderStorageImageArrayDynamicIndexing ? "true" : "false"));
    LOG_TRACE("\tshaderClipDistance: %s", (pFeatures.features.shaderClipDistance ? "true" : "false"));
    LOG_TRACE("\tshaderCullDistance: %s", (pFeatures.features.shaderCullDistance ? "true" : "false"));
    LOG_TRACE("\tshaderFloat64: %s", (pFeatures.features.shaderFloat64 ? "true" : "false"));
    LOG_TRACE("\tshaderInt64: %s", (pFeatures.features.shaderInt64 ? "true" : "false"));
    LOG_TRACE("\tshaderInt16: %s", (pFeatures.features.shaderInt16 ? "true" : "false"));
    LOG_TRACE("\tshaderResourceResidency: %s", (pFeatures.features.shaderResourceResidency ? "true" : "false"));
    LOG_TRACE("\tshaderResourceMinLod: %s", (pFeatures.features.shaderResourceMinLod ? "true" : "false"));
    LOG_TRACE("\tsparseBinding: %s", (pFeatures.features.sparseBinding ? "true" : "false"));
    LOG_TRACE("\tsparseResidencyBuffer: %s", (pFeatures.features.sparseResidencyBuffer ? "true" : "false"));
    LOG_TRACE("\tsparseResidencyImage2D: %s", (pFeatures.features.sparseResidencyImage2D ? "true" : "false"));
    LOG_TRACE("\tsparseResidencyImage3D: %s", (pFeatures.features.sparseResidencyImage3D ? "true" : "false"));
    LOG_TRACE("\tsparseResidency2Samples: %s", (pFeatures.features.sparseResidency2Samples ? "true" : "false"));
    LOG_TRACE("\tsparseResidency4Samples: %s", (pFeatures.features.sparseResidency4Samples ? "true" : "false"));
    LOG_TRACE("\tsparseResidency8Samples: %s", (pFeatures.features.sparseResidency8Samples ? "true" : "false"));
    LOG_TRACE("\tsparseResidency16Samples: %s", (pFeatures.features.sparseResidency16Samples ? "true" : "false"));
    LOG_TRACE("\tsparseResidencyAliased: %s", (pFeatures.features.sparseResidencyAliased ? "true" : "false"));
    LOG_TRACE("\tvariableMultisampleRate: %s", (pFeatures.features.variableMultisampleRate ? "true" : "false"));
    LOG_TRACE("\tinheritedQueries: %s", (pFeatures.features.inheritedQueries ? "true" : "false"));
}

const void VulkanPhysicalDevice::PrintDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties pMemoryProperties) const
{
    FN("VulkanPhysicalDevice::PrintDeviceMemoryProperties");

    LOG_TRACE("MEMORY TYPES:");
    for (size_t m = 0; m < pMemoryProperties.memoryTypeCount; m++)
    {
        LOG_TRACE("\tmemoryTypes[%u].heapIndex: %u", m, pMemoryProperties.memoryTypes[m].heapIndex);
        std::string memoryFlagsString = "";
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_HOST_COHERENT_BIT");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_HOST_CACHED_BIT");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_PROTECTED_BIT");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD");
        if (pMemoryProperties.memoryTypes[m].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD)
            memoryFlagsString.append("\n\t\tVK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD");
        LOG_TRACE("\tmemoryTypes[%u].propertyFlags: %s", m, memoryFlagsString.c_str());
    }

    LOG_TRACE("MEMORY HEAPS:");
    for (size_t m = 0; m < pMemoryProperties.memoryHeapCount; m++)
    {
        LOG_TRACE("\tmemoryHeaps[%u].size: %u", m, pMemoryProperties.memoryHeaps[m].size);
        std::string memoryFlagsString = "";
        if (pMemoryProperties.memoryHeaps[m].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_HEAP_DEVICE_LOCAL_BIT");
        if (pMemoryProperties.memoryHeaps[m].flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT)
            memoryFlagsString.append("\n\t\tVK_MEMORY_HEAP_MULTI_INSTANCE_BIT");
        if (pMemoryProperties.memoryHeaps[m].flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR)
            memoryFlagsString.append("\n\t\tVK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR");
        LOG_TRACE("\tmemoryHeaps[%u].flags: %s", m, memoryFlagsString.c_str());
    }
}

const void VulkanPhysicalDevice::PrintDeviceQueueProperties(const std::vector<VkQueueFamilyProperties> &queue_properties) const
{
    FN("VulkanPhysicalDevice::PrintDeviceQueueProperties");

    LOG_TRACE("QUEUES:");
    for (size_t k = 0; k < queue_properties.size(); k++)
    {
        LOG_TRACE("\tQueue %u:", k);
        if (queue_properties[k].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            LOG_TRACE("\t\tVK_QUEUE_GRAPHICS_BIT");
        }

        if (queue_properties[k].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            LOG_TRACE("\t\tVK_QUEUE_COMPUTE_BIT");
        }

        if (queue_properties[k].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            LOG_TRACE("\t\tVK_QUEUE_TRANSFER_BIT");
        }

        if (queue_properties[k].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
        {
            LOG_TRACE("\t\tVK_QUEUE_SPARSE_BINDING_BIT");
        }

        if (queue_properties[k].queueFlags & VK_QUEUE_PROTECTED_BIT)
        {
            LOG_TRACE("\t\tVK_QUEUE_PROTECTED_BIT");
        }

        LOG_TRACE("\t\tqueueCount: %u", queue_properties[k].queueCount);
        LOG_TRACE("\t\ttimestampValidBits: %u", queue_properties[k].timestampValidBits);
        LOG_TRACE("\t\tminImageTransferGranularity: w: %u h: %u d: %u", queue_properties[k].minImageTransferGranularity.width, queue_properties[k].minImageTransferGranularity.height, queue_properties[k].minImageTransferGranularity.depth);
    }
}

const void VulkanPhysicalDevice::PrintSurfaceCapabilities(VkSurfaceCapabilitiesKHR surfaceCapabilities) const
{
    FN("VulkanPhysicalDevice::PrintSurfaceCapabilities");

    LOG_TRACE("SURFACE CAPABILITIES:");
    LOG_TRACE("\tminImageCount: %u", surfaceCapabilities.minImageCount);
    LOG_TRACE("\tmaxImageCount: %u", surfaceCapabilities.maxImageCount);
    LOG_TRACE("\tcurrentExtent: w: %u h: %u", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
    LOG_TRACE("\tminImageExtent: w: %u h: %u", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height);
    LOG_TRACE("\tmaxImageExtent: w: %u h: %u", surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
    LOG_TRACE("\tmaxImageArrayLayers: %u", surfaceCapabilities.maxImageArrayLayers);

    std::string supportedTransformsString = "";
    if (surfaceCapabilities.supportedTransforms & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
        supportedTransformsString.append("\n\t\tVK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR");
    if (surfaceCapabilities.supportedTransforms & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        supportedTransformsString.append("\n\t\tVK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR");
    if (surfaceCapabilities.supportedTransforms & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
        supportedTransformsString.append("\n\t\tVK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR");
    if (surfaceCapabilities.supportedTransforms & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        supportedTransformsString.append("\n\t\tVK_COMPOSITE_ALPHA_INHERIT_BIT_KHR");
    LOG_TRACE("\tsupportedTransforms: %s", supportedTransformsString.c_str());

    std::string currentTransformString = "";
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR");
    if (surfaceCapabilities.currentTransform & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR)
        currentTransformString.append("\n\t\tVK_SURFACE_TRANSFORM_INHERIT_BIT_KHR");
    LOG_TRACE("\tcurrentTransform: %s", currentTransformString.c_str());

    std::string supportedCompositeAlphaString = "";
    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
        supportedCompositeAlphaString.append("\n\t\tVK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR");
    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        supportedCompositeAlphaString.append("\n\t\tVK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR");
    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
        supportedCompositeAlphaString.append("\n\t\tVK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR");
    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        supportedCompositeAlphaString.append("\n\t\tVK_COMPOSITE_ALPHA_INHERIT_BIT_KHR");
    LOG_TRACE("\tsupportedCompositeAlpha: %s", supportedCompositeAlphaString.c_str());

    std::string supportedUsageFlagsString = "";
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_TRANSFER_SRC_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_TRANSFER_DST_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_SAMPLED_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_STORAGE_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT");
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR)
        supportedUsageFlagsString.append("\n\t\tVK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR");
    LOG_TRACE("\tsupportedUsageFlags: %s", supportedUsageFlagsString.c_str());
}

const void VulkanPhysicalDevice::PrintSurfaceFormats(std::vector<VkSurfaceFormatKHR> surfaceFormats) const
{
    FN("VulkanPhysicalDevice::PrintSurfaceFormats");

    LOG_TRACE("SURFACE FORMATS:");
    for (size_t i = 0; i < surfaceFormats.size(); i++)
    {
        LOG_TRACE("\tformat[%u].format: %s", i, VkFormatToString(surfaceFormats[i].format).c_str());
        LOG_TRACE("\tformat[%u].colorSpace: %s", i, VkColorSpaceToString(surfaceFormats[i].colorSpace).c_str());
    }
}

const void VulkanPhysicalDevice::PrintSurfacePresentationModes(std::vector<VkPresentModeKHR> surfacePresentationModes) const
{
    FN("VulkanPhysicalDevice::PrintSurfacePresentationModes");

    LOG_TRACE("SURFACE PRESENTATION MODES:");
    for (size_t i = 0; i < surfacePresentationModes.size(); i++)
    {
        if (surfacePresentationModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
            LOG_TRACE("\tVK_PRESENT_MODE_IMMEDIATE_KHR");
        if (surfacePresentationModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            LOG_TRACE("\tVK_PRESENT_MODE_MAILBOX_KHR");
        if (surfacePresentationModes[i] == VK_PRESENT_MODE_FIFO_KHR)
            LOG_TRACE("\tVK_PRESENT_MODE_FIFO_KHR");
        if (surfacePresentationModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            LOG_TRACE("\tVK_PRESENT_MODE_FIFO_RELAXED_KHR");
        if (surfacePresentationModes[i] == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR)
            LOG_TRACE("\tVK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR");
        if (surfacePresentationModes[i] == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR)
            LOG_TRACE("\tVK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR");
    }
}

const std::string VulkanPhysicalDevice::VkFormatToString(VkFormat format) const
{
    FN("VulkanPhysicalDevice::VkFormatToString");

    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return "VK_FORMAT_UNDEFINED";
    case VK_FORMAT_R4G4_UNORM_PACK8:
        return "VK_FORMAT_R4G4_UNORM_PACK8";
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
        return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return "VK_FORMAT_R5G6B5_UNORM_PACK16";
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
        return "VK_FORMAT_B5G6R5_UNORM_PACK16";
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
        return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
        return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
    case VK_FORMAT_R8_UNORM:
        return "VK_FORMAT_R8_UNORM";
    case VK_FORMAT_R8_SNORM:
        return "VK_FORMAT_R8_SNORM";
    case VK_FORMAT_R8_USCALED:
        return "VK_FORMAT_R8_USCALED";
    case VK_FORMAT_R8_SSCALED:
        return "VK_FORMAT_R8_SSCALED";
    case VK_FORMAT_R8_UINT:
        return "VK_FORMAT_R8_UINT";
    case VK_FORMAT_R8_SINT:
        return "VK_FORMAT_R8_SINT";
    case VK_FORMAT_R8_SRGB:
        return "VK_FORMAT_R8_SRGB";
    case VK_FORMAT_R8G8_UNORM:
        return "VK_FORMAT_R8G8_UNORM";
    case VK_FORMAT_R8G8_SNORM:
        return "VK_FORMAT_R8G8_SNORM";
    case VK_FORMAT_R8G8_USCALED:
        return "VK_FORMAT_R8G8_USCALED";
    case VK_FORMAT_R8G8_SSCALED:
        return "VK_FORMAT_R8G8_SSCALED";
    case VK_FORMAT_R8G8_UINT:
        return "VK_FORMAT_R8G8_UINT";
    case VK_FORMAT_R8G8_SINT:
        return "VK_FORMAT_R8G8_SINT";
    case VK_FORMAT_R8G8_SRGB:
        return "VK_FORMAT_R8G8_SRGB";
    case VK_FORMAT_R8G8B8_UNORM:
        return "VK_FORMAT_R8G8B8_UNORM";
    case VK_FORMAT_R8G8B8_SNORM:
        return "VK_FORMAT_R8G8B8_SNORM";
    case VK_FORMAT_R8G8B8_USCALED:
        return "VK_FORMAT_R8G8B8_USCALED";
    case VK_FORMAT_R8G8B8_SSCALED:
        return "VK_FORMAT_R8G8B8_SSCALED";
    case VK_FORMAT_R8G8B8_UINT:
        return "VK_FORMAT_R8G8B8_UINT";
    case VK_FORMAT_R8G8B8_SINT:
        return "VK_FORMAT_R8G8B8_SINT";
    case VK_FORMAT_R8G8B8_SRGB:
        return "VK_FORMAT_R8G8B8_SRGB";
    case VK_FORMAT_B8G8R8_UNORM:
        return "VK_FORMAT_B8G8R8_UNORM";
    case VK_FORMAT_B8G8R8_SNORM:
        return "VK_FORMAT_B8G8R8_SNORM";
    case VK_FORMAT_B8G8R8_USCALED:
        return "VK_FORMAT_B8G8R8_USCALED";
    case VK_FORMAT_B8G8R8_SSCALED:
        return "VK_FORMAT_B8G8R8_SSCALED";
    case VK_FORMAT_B8G8R8_UINT:
        return "VK_FORMAT_B8G8R8_UINT";
    case VK_FORMAT_B8G8R8_SINT:
        return "VK_FORMAT_B8G8R8_SINT";
    case VK_FORMAT_B8G8R8_SRGB:
        return "VK_FORMAT_B8G8R8_SRGB";
    case VK_FORMAT_R8G8B8A8_UNORM:
        return "VK_FORMAT_R8G8B8A8_UNORM";
    case VK_FORMAT_R8G8B8A8_SNORM:
        return "VK_FORMAT_R8G8B8A8_SNORM";
    case VK_FORMAT_R8G8B8A8_USCALED:
        return "VK_FORMAT_R8G8B8A8_USCALED";
    case VK_FORMAT_R8G8B8A8_SSCALED:
        return "VK_FORMAT_R8G8B8A8_SSCALED";
    case VK_FORMAT_R8G8B8A8_UINT:
        return "VK_FORMAT_R8G8B8A8_UINT";
    case VK_FORMAT_R8G8B8A8_SINT:
        return "VK_FORMAT_R8G8B8A8_SINT";
    case VK_FORMAT_R8G8B8A8_SRGB:
        return "VK_FORMAT_R8G8B8A8_SRGB";
    case VK_FORMAT_B8G8R8A8_UNORM:
        return "VK_FORMAT_B8G8R8A8_UNORM";
    case VK_FORMAT_B8G8R8A8_SNORM:
        return "VK_FORMAT_B8G8R8A8_SNORM";
    case VK_FORMAT_B8G8R8A8_USCALED:
        return "VK_FORMAT_B8G8R8A8_USCALED";
    case VK_FORMAT_B8G8R8A8_SSCALED:
        return "VK_FORMAT_B8G8R8A8_SSCALED";
    case VK_FORMAT_B8G8R8A8_UINT:
        return "VK_FORMAT_B8G8R8A8_UINT";
    case VK_FORMAT_B8G8R8A8_SINT:
        return "VK_FORMAT_B8G8R8A8_SINT";
    case VK_FORMAT_B8G8R8A8_SRGB:
        return "VK_FORMAT_B8G8R8A8_SRGB";
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
        return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
        return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
        return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
        return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
        return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
        return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
        return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
        return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
        return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
        return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
        return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
        return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
        return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
    case VK_FORMAT_R16_UNORM:
        return "VK_FORMAT_R16_UNORM";
    case VK_FORMAT_R16_SNORM:
        return "VK_FORMAT_R16_SNORM";
    case VK_FORMAT_R16_USCALED:
        return "VK_FORMAT_R16_USCALED";
    case VK_FORMAT_R16_SSCALED:
        return "VK_FORMAT_R16_SSCALED";
    case VK_FORMAT_R16_UINT:
        return "VK_FORMAT_R16_UINT";
    case VK_FORMAT_R16_SINT:
        return "VK_FORMAT_R16_SINT";
    case VK_FORMAT_R16_SFLOAT:
        return "VK_FORMAT_R16_SFLOAT";
    case VK_FORMAT_R16G16_UNORM:
        return "VK_FORMAT_R16G16_UNORM";
    case VK_FORMAT_R16G16_SNORM:
        return "VK_FORMAT_R16G16_SNORM";
    case VK_FORMAT_R16G16_USCALED:
        return "VK_FORMAT_R16G16_USCALED";
    case VK_FORMAT_R16G16_SSCALED:
        return "VK_FORMAT_R16G16_SSCALED";
    case VK_FORMAT_R16G16_UINT:
        return "VK_FORMAT_R16G16_UINT";
    case VK_FORMAT_R16G16_SINT:
        return "VK_FORMAT_R16G16_SINT";
    case VK_FORMAT_R16G16_SFLOAT:
        return "VK_FORMAT_R16G16_SFLOAT";
    case VK_FORMAT_R16G16B16_UNORM:
        return "VK_FORMAT_R16G16B16_UNORM";
    case VK_FORMAT_R16G16B16_SNORM:
        return "VK_FORMAT_R16G16B16_SNORM";
    case VK_FORMAT_R16G16B16_USCALED:
        return "VK_FORMAT_R16G16B16_USCALED";
    case VK_FORMAT_R16G16B16_SSCALED:
        return "VK_FORMAT_R16G16B16_SSCALED";
    case VK_FORMAT_R16G16B16_UINT:
        return "VK_FORMAT_R16G16B16_UINT";
    case VK_FORMAT_R16G16B16_SINT:
        return "VK_FORMAT_R16G16B16_SINT";
    case VK_FORMAT_R16G16B16_SFLOAT:
        return "VK_FORMAT_R16G16B16_SFLOAT";
    case VK_FORMAT_R16G16B16A16_UNORM:
        return "VK_FORMAT_R16G16B16A16_UNORM";
    case VK_FORMAT_R16G16B16A16_SNORM:
        return "VK_FORMAT_R16G16B16A16_SNORM";
    case VK_FORMAT_R16G16B16A16_USCALED:
        return "VK_FORMAT_R16G16B16A16_USCALED";
    case VK_FORMAT_R16G16B16A16_SSCALED:
        return "VK_FORMAT_R16G16B16A16_SSCALED";
    case VK_FORMAT_R16G16B16A16_UINT:
        return "VK_FORMAT_R16G16B16A16_UINT";
    case VK_FORMAT_R16G16B16A16_SINT:
        return "VK_FORMAT_R16G16B16A16_SINT";
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return "VK_FORMAT_R16G16B16A16_SFLOAT";
    case VK_FORMAT_R32_UINT:
        return "VK_FORMAT_R32_UINT";
    case VK_FORMAT_R32_SINT:
        return "VK_FORMAT_R32_SINT";
    case VK_FORMAT_R32_SFLOAT:
        return "VK_FORMAT_R32_SFLOAT";
    case VK_FORMAT_R32G32_UINT:
        return "VK_FORMAT_R32G32_UINT";
    case VK_FORMAT_R32G32_SINT:
        return "VK_FORMAT_R32G32_SINT";
    case VK_FORMAT_R32G32_SFLOAT:
        return "VK_FORMAT_R32G32_SFLOAT";
    case VK_FORMAT_R32G32B32_UINT:
        return "VK_FORMAT_R32G32B32_UINT";
    case VK_FORMAT_R32G32B32_SINT:
        return "VK_FORMAT_R32G32B32_SINT";
    case VK_FORMAT_R32G32B32_SFLOAT:
        return "VK_FORMAT_R32G32B32_SFLOAT";
    case VK_FORMAT_R32G32B32A32_UINT:
        return "VK_FORMAT_R32G32B32A32_UINT";
    case VK_FORMAT_R32G32B32A32_SINT:
        return "VK_FORMAT_R32G32B32A32_SINT";
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return "VK_FORMAT_R32G32B32A32_SFLOAT";
    case VK_FORMAT_R64_UINT:
        return "VK_FORMAT_R64_UINT";
    case VK_FORMAT_R64_SINT:
        return "VK_FORMAT_R64_SINT";
    case VK_FORMAT_R64_SFLOAT:
        return "VK_FORMAT_R64_SFLOAT";
    case VK_FORMAT_R64G64_UINT:
        return "VK_FORMAT_R64G64_UINT";
    case VK_FORMAT_R64G64_SINT:
        return "VK_FORMAT_R64G64_SINT";
    case VK_FORMAT_R64G64_SFLOAT:
        return "VK_FORMAT_R64G64_SFLOAT";
    case VK_FORMAT_R64G64B64_UINT:
        return "VK_FORMAT_R64G64B64_UINT";
    case VK_FORMAT_R64G64B64_SINT:
        return "VK_FORMAT_R64G64B64_SINT";
    case VK_FORMAT_R64G64B64_SFLOAT:
        return "VK_FORMAT_R64G64B64_SFLOAT";
    case VK_FORMAT_R64G64B64A64_UINT:
        return "VK_FORMAT_R64G64B64A64_UINT";
    case VK_FORMAT_R64G64B64A64_SINT:
        return "VK_FORMAT_R64G64B64A64_SINT";
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return "VK_FORMAT_R64G64B64A64_SFLOAT";
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
        return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
    case VK_FORMAT_D16_UNORM:
        return "VK_FORMAT_D16_UNORM";
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return "VK_FORMAT_X8_D24_UNORM_PACK32";
    case VK_FORMAT_D32_SFLOAT:
        return "VK_FORMAT_D32_SFLOAT";
    case VK_FORMAT_S8_UINT:
        return "VK_FORMAT_S8_UINT";
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return "VK_FORMAT_D16_UNORM_S8_UINT";
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return "VK_FORMAT_D24_UNORM_S8_UINT";
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return "VK_FORMAT_D32_SFLOAT_S8_UINT";
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
    case VK_FORMAT_BC2_UNORM_BLOCK:
        return "VK_FORMAT_BC2_UNORM_BLOCK";
    case VK_FORMAT_BC2_SRGB_BLOCK:
        return "VK_FORMAT_BC2_SRGB_BLOCK";
    case VK_FORMAT_BC3_UNORM_BLOCK:
        return "VK_FORMAT_BC3_UNORM_BLOCK";
    case VK_FORMAT_BC3_SRGB_BLOCK:
        return "VK_FORMAT_BC3_SRGB_BLOCK";
    case VK_FORMAT_BC4_UNORM_BLOCK:
        return "VK_FORMAT_BC4_UNORM_BLOCK";
    case VK_FORMAT_BC4_SNORM_BLOCK:
        return "VK_FORMAT_BC4_SNORM_BLOCK";
    case VK_FORMAT_BC5_UNORM_BLOCK:
        return "VK_FORMAT_BC5_UNORM_BLOCK";
    case VK_FORMAT_BC5_SNORM_BLOCK:
        return "VK_FORMAT_BC5_SNORM_BLOCK";
    case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
    case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
    case VK_FORMAT_BC7_UNORM_BLOCK:
        return "VK_FORMAT_BC7_UNORM_BLOCK";
    case VK_FORMAT_BC7_SRGB_BLOCK:
        return "VK_FORMAT_BC7_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
    case VK_FORMAT_EAC_R11_UNORM_BLOCK:
        return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
    case VK_FORMAT_EAC_R11_SNORM_BLOCK:
        return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
        return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
        return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
    case VK_FORMAT_G8B8G8R8_422_UNORM:
        return "VK_FORMAT_G8B8G8R8_422_UNORM";
    case VK_FORMAT_B8G8R8G8_422_UNORM:
        return "VK_FORMAT_B8G8R8G8_422_UNORM";
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
    case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
    case VK_FORMAT_R10X6_UNORM_PACK16:
        return "VK_FORMAT_R10X6_UNORM_PACK16";
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
        return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
        return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
        return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
        return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
    case VK_FORMAT_R12X4_UNORM_PACK16:
        return "VK_FORMAT_R12X4_UNORM_PACK16";
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
        return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
        return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
        return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
        return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
    case VK_FORMAT_G16B16G16R16_422_UNORM:
        return "VK_FORMAT_G16B16G16R16_422_UNORM";
    case VK_FORMAT_B16G16R16G16_422_UNORM:
        return "VK_FORMAT_B16G16R16G16_422_UNORM";
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
    case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:
        return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT";
    case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:
        return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT";
    case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
        return "VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT";
    case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
        return "VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT";
    case VK_FORMAT_MAX_ENUM:
        return "VK_FORMAT_MAX_ENUM";
    default:
        return "Unknown VkFormat";
    }
}

const std::string VulkanPhysicalDevice::VkColorSpaceToString(VkColorSpaceKHR colorSpace) const
{
    FN("VulkanPhysicalDevice::VkColorSpaceToString");

    switch (colorSpace)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR";
    case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
        return "VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT";
    case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        return "VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT";
    case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:
        return "VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT";
    case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
        return "VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT";
    case VK_COLOR_SPACE_BT709_LINEAR_EXT:
        return "VK_COLOR_SPACE_BT709_LINEAR_EXT";
    case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
        return "VK_COLOR_SPACE_BT709_NONLINEAR_EXT";
    case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
        return "VK_COLOR_SPACE_BT2020_LINEAR_EXT";
    case VK_COLOR_SPACE_HDR10_ST2084_EXT:
        return "VK_COLOR_SPACE_HDR10_ST2084_EXT";
    case VK_COLOR_SPACE_DOLBYVISION_EXT:
        return "VK_COLOR_SPACE_DOLBYVISION_EXT";
    case VK_COLOR_SPACE_HDR10_HLG_EXT:
        return "VK_COLOR_SPACE_HDR10_HLG_EXT";
    case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
        return "VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT";
    case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
        return "VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT";
    case VK_COLOR_SPACE_PASS_THROUGH_EXT:
        return "VK_COLOR_SPACE_PASS_THROUGH_EXT";
    case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
        return "VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT";
    case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:
        return "VK_COLOR_SPACE_DISPLAY_NATIVE_AMD";
    case VK_COLOR_SPACE_MAX_ENUM_KHR:
        return "VK_COLOR_SPACE_MAX_ENUM_KHR";
    default:
        return "Unknown VkColorSpaceKHR";
    }
}
} // namespace bow