#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>

#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanCommandPool.h>
#include <VulkanRenderDevice/Device/BowVulkanFence.h>
#include <VulkanRenderDevice/Device/BowVulkanSemaphore.h>
#include <VulkanRenderDevice/Device/BowVulkanSwapchain.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanIndexBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanStorageBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanUniformBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanVertexBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanBottomLevelAccelerationStructure.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanRayTracingShaderProgram.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanTopLevelAccelerationStructure.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderProgram.h>
#include <VulkanRenderDevice/VulkanLoaderLibrary.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bow
{

VulkanLogicalDevice::VulkanLogicalDevice()
    : m_physicalDevice(nullptr), m_device(nullptr), m_graphicsQueue(nullptr), m_presentQueue(nullptr), m_computeQueue(nullptr), m_transferQueue(nullptr), m_presentQueueFamilyIndex(UINT32_MAX), m_graphicsQueueFamilyIndex(UINT32_MAX),
      m_computeQueueFamilyIndex(UINT32_MAX), m_transferQueueFamilyIndex(UINT32_MAX), m_guid(Utils::GenerateGUID())
{
    FN("VulkanLogicalDevice::VulkanLogicalDevice");
}

VulkanLogicalDevice::~VulkanLogicalDevice()
{
    FN("VulkanLogicalDevice::~VulkanLogicalDevice");

    Release();
}

bool VulkanLogicalDevice::Initialize(VulkanPhysicalDevice *physicalDevice, uint32_t graphicsFamilyIdx, uint32_t presentFamilyIdx, uint32_t computeFamilyIndex, uint32_t transferFamilyIndex)
{
    FN("VulkanLogicalDevice::Initialize");

    m_physicalDevice = physicalDevice;
    m_graphicsQueueFamilyIndex = graphicsFamilyIdx;
    m_presentQueueFamilyIndex = presentFamilyIdx;
    m_computeQueueFamilyIndex = computeFamilyIndex;
    m_transferQueueFamilyIndex = transferFamilyIndex;

    std::unordered_map<uint32_t, uint32_t> indexFamilyQueueMap;
    std::unordered_map<uint32_t, std::vector<float>> queuePrioritiesMap;

    // Populate the map with the initial queue family counts
    auto incrementQueueFamily = [&indexFamilyQueueMap, &queuePrioritiesMap](uint32_t familyIndex, float priority)
    {
        if (familyIndex != UINT32_MAX)
        {
            if (indexFamilyQueueMap.find(familyIndex) == indexFamilyQueueMap.end())
            {
                indexFamilyQueueMap[familyIndex] = 1;
            }
            else
            {
                indexFamilyQueueMap[familyIndex]++;
            }
            queuePrioritiesMap[familyIndex].push_back(priority);
        }
    };

    incrementQueueFamily(m_graphicsQueueFamilyIndex, 1.0f);
    incrementQueueFamily(m_presentQueueFamilyIndex, 1.0f);
    incrementQueueFamily(m_computeQueueFamilyIndex, 0.0f);
    incrementQueueFamily(m_transferQueueFamilyIndex, 0.0f);

    // Prepare to create the logical device.
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    for (const auto &queueFamily : indexFamilyQueueMap)
    {
        VkDeviceQueueCreateInfo queue_create_info = {};
        memset(&queue_create_info, 0, sizeof(queue_create_info));
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.pNext = nullptr;
        queue_create_info.flags = 0;
        queue_create_info.queueFamilyIndex = queueFamily.first;
        queue_create_info.queueCount = queueFamily.second;
        queue_create_info.pQueuePriorities = queuePrioritiesMap[queueFamily.first].data();
        queue_create_infos.push_back(queue_create_info);
    }

    const std::vector<const char *> deviceLayer = {};                               // No layers enabled (deprecated)
    std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}; // Enable extensions as needed

    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures = {};
    VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR positionFetchFeatures = {};
    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferDeviceAddressFeatures = {};
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT deviceDescriptorIndexingFeatures = {};

    void *pNextFeatures = nullptr;
    // Enable ray tracing extensions if needed
    bool m_rayTracingEnabled = true;
    if (m_rayTracingEnabled)
    {
        positionFetchFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR;
        positionFetchFeatures.pNext = nullptr;
        positionFetchFeatures.rayTracingPositionFetch = VK_TRUE;

        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelerationStructureFeatures.pNext = &positionFetchFeatures;
        accelerationStructureFeatures.accelerationStructure = VK_TRUE;
        accelerationStructureFeatures.accelerationStructureCaptureReplay = VK_FALSE;
        accelerationStructureFeatures.accelerationStructureHostCommands = VK_FALSE;
        accelerationStructureFeatures.accelerationStructureIndirectBuild = VK_FALSE;

        rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        rayTracingPipelineFeatures.pNext = &accelerationStructureFeatures;
        rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
        rayTracingPipelineFeatures.rayTracingPipelineShaderGroupHandleCaptureReplay = VK_FALSE;
        rayTracingPipelineFeatures.rayTracingPipelineShaderGroupHandleCaptureReplayMixed = VK_FALSE;
        rayTracingPipelineFeatures.rayTracingPipelineTraceRaysIndirect = VK_FALSE;
        rayTracingPipelineFeatures.rayTraversalPrimitiveCulling = VK_FALSE;

        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
        bufferDeviceAddressFeatures.pNext = &rayTracingPipelineFeatures;
        bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;
        bufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay = VK_FALSE;
        bufferDeviceAddressFeatures.bufferDeviceAddressMultiDevice = VK_FALSE;

        deviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        deviceDescriptorIndexingFeatures.pNext = &bufferDeviceAddressFeatures;
        deviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
        deviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
        deviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        deviceDescriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;

        pNextFeatures = &deviceDescriptorIndexingFeatures;

        deviceExtensions.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME);
        /*
        deviceExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
        */
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
    physicalDeviceFeatures.robustBufferAccess = VK_FALSE;                      // enables robust buffer access
    physicalDeviceFeatures.fullDrawIndexUint32 = VK_FALSE;                     // enables full 32-bit range of indices for indexed draw calls
    physicalDeviceFeatures.imageCubeArray = VK_FALSE;                          // enables creation of arrays of 2D images
    physicalDeviceFeatures.independentBlend = VK_FALSE;                        // enables independent blending of color channels
    physicalDeviceFeatures.geometryShader = VK_TRUE;                           // enables geometry shaders
    physicalDeviceFeatures.tessellationShader = VK_TRUE;                       // enables tessellation shaders
    physicalDeviceFeatures.sampleRateShading = VK_FALSE;                       // enables per-sample shading and interpolation
    physicalDeviceFeatures.dualSrcBlend = VK_FALSE;                            // enables dual-source blending
    physicalDeviceFeatures.logicOp = VK_FALSE;                                 // enables logic operations
    physicalDeviceFeatures.multiDrawIndirect = VK_FALSE;                       // enables multiple draw indirect
    physicalDeviceFeatures.drawIndirectFirstInstance = VK_FALSE;               // enables draw indirect with first instance
    physicalDeviceFeatures.depthClamp = VK_TRUE;                               // enables depth clamping
    physicalDeviceFeatures.depthBiasClamp = VK_FALSE;                          // enables depth bias clamping
    physicalDeviceFeatures.fillModeNonSolid = VK_FALSE;                        // enables non-solid fill modes
    physicalDeviceFeatures.depthBounds = VK_FALSE;                             // enables depth bounds test
    physicalDeviceFeatures.wideLines = VK_FALSE;                               // enables wide lines
    physicalDeviceFeatures.largePoints = VK_FALSE;                             // enables large points
    physicalDeviceFeatures.alphaToOne = VK_FALSE;                              // enables alpha to one
    physicalDeviceFeatures.multiViewport = VK_FALSE;                           // enables multi viewport support
    physicalDeviceFeatures.samplerAnisotropy = VK_FALSE;                       // enables sampler anisotropy
    physicalDeviceFeatures.textureCompressionETC2 = VK_FALSE;                  // enables ETC texture compression formats
    physicalDeviceFeatures.textureCompressionASTC_LDR = VK_FALSE;              // enables ASTC LDR texture compression formats
    physicalDeviceFeatures.textureCompressionBC = VK_FALSE;                    // enables BC texture compression formats
    physicalDeviceFeatures.occlusionQueryPrecise = VK_FALSE;                   // requires precise occlusion queries
    physicalDeviceFeatures.pipelineStatisticsQuery = VK_FALSE;                 // enables pipeline statistics query
    physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;           // enables stores and atomic ops on storage buffers and images
    physicalDeviceFeatures.fragmentStoresAndAtomics = VK_TRUE;                 // enables stores and atomic ops on storage buffers and images
    physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;  // enables tessellation and geometry stages to output point size
    physicalDeviceFeatures.shaderImageGatherExtended = VK_FALSE;               // enables image gather with run-time values and independent offsets
    physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;        // enables extended formats for storage images
    physicalDeviceFeatures.shaderStorageImageMultisample = VK_FALSE;           // enables multisampled storage images
    physicalDeviceFeatures.shaderStorageImageReadWithoutFormat = VK_FALSE;     // enables reading storage images without format
    physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat = VK_FALSE;    // enables writing storage images without format
    physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_FALSE; // enables arrays of uniform buffers with dynamic indexing
    physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_FALSE;  // enables arrays of sampled images with dynamic indexing
    physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_FALSE; // enables arrays of storage buffers with dynamic indexing
    physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_FALSE;  // enables arrays of storage images with dynamic indexing
    physicalDeviceFeatures.shaderClipDistance = VK_FALSE;                      // enables shader clip distance
    physicalDeviceFeatures.shaderCullDistance = VK_FALSE;                      // enables shader cull distance
    physicalDeviceFeatures.shaderFloat64 = VK_FALSE;                           // enables 64-bit floats
    physicalDeviceFeatures.shaderInt64 = VK_FALSE;                             // enables 64-bit integers
    physicalDeviceFeatures.shaderInt16 = VK_FALSE;                             // enables 16-bit integers
    physicalDeviceFeatures.shaderResourceResidency = VK_FALSE;                 // enables resource residency
    physicalDeviceFeatures.shaderResourceMinLod = VK_FALSE;                    // enables resource min LOD
    physicalDeviceFeatures.sparseBinding = VK_FALSE;                           // enables sparse binding
    physicalDeviceFeatures.sparseResidencyBuffer = VK_FALSE;                   // enables sparse buffer residency
    physicalDeviceFeatures.sparseResidencyImage2D = VK_FALSE;                  // enables sparse 2D image residency
    physicalDeviceFeatures.sparseResidencyImage3D = VK_FALSE;                  // enables sparse 3D image residency
    physicalDeviceFeatures.sparseResidency2Samples = VK_FALSE;                 // enables sparse MSAA 2 samples
    physicalDeviceFeatures.sparseResidency4Samples = VK_FALSE;                 // enables sparse MSAA 4 samples
    physicalDeviceFeatures.sparseResidency8Samples = VK_FALSE;                 // enables sparse MSAA 8 samples
    physicalDeviceFeatures.sparseResidency16Samples = VK_FALSE;                // enables sparse MSAA 16 samples
    physicalDeviceFeatures.sparseResidencyAliased = VK_FALSE;                  // enables aliased residency
    physicalDeviceFeatures.variableMultisampleRate = VK_FALSE;                 // enables variable multisample rate
    physicalDeviceFeatures.inheritedQueries = VK_FALSE;                        // enables inherited queries

    // Set up the device creation info.
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = pNextFeatures;
    deviceCreateInfo.queueCreateInfoCount = queue_create_infos.size();
    deviceCreateInfo.pQueueCreateInfos = queue_create_infos.data();
    deviceCreateInfo.enabledLayerCount = deviceLayer.size();
    deviceCreateInfo.ppEnabledLayerNames = deviceLayer.data();
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    // Create the logical device.
    LOG_TRACE("vkCreateDevice %s", m_guid.c_str());
    VkResult result = vkCreateDevice(m_physicalDevice->GetHandle(), &deviceCreateInfo, nullptr, &m_device);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanLogicalDevice: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    LoadDeviceLevelFunctions(m_device, deviceExtensions);

    // Reset the queue indices in the map
    for (auto &queueFamily : indexFamilyQueueMap)
    {
        queueFamily.second = 0;
    }

    // Retrieve the queues
    auto getDeviceQueue = [this, &indexFamilyQueueMap](uint32_t familyIndex, VkQueue &queue)
    {
        if (familyIndex != UINT32_MAX)
        {
            LOG_TRACE("vkGetDeviceQueue %s", m_guid.c_str());
            vkGetDeviceQueue(m_device, familyIndex, indexFamilyQueueMap[familyIndex]++, &queue);
        }
    };

    getDeviceQueue(m_graphicsQueueFamilyIndex, m_graphicsQueue);
    getDeviceQueue(m_presentQueueFamilyIndex, m_presentQueue);
    getDeviceQueue(m_computeQueueFamilyIndex, m_computeQueue);
    getDeviceQueue(m_transferQueueFamilyIndex, m_transferQueue);

    m_graphicsCommandPool = VulkanCommandPoolPtr(new VulkanCommandPool());
    m_graphicsCommandPool->Initialize(this, m_graphicsQueueFamilyIndex);

    m_presentCommandPool = VulkanCommandPoolPtr(new VulkanCommandPool());
    m_presentCommandPool->Initialize(this, m_presentQueueFamilyIndex);

    m_computeCommandPool = VulkanCommandPoolPtr(new VulkanCommandPool());
    m_computeCommandPool->Initialize(this, m_computeQueueFamilyIndex);

    m_transferCommandPool = VulkanCommandPoolPtr(new VulkanCommandPool());
    m_transferCommandPool->Initialize(this, m_transferQueueFamilyIndex);

    return true;
}

void VulkanLogicalDevice::Release()
{
    FN("VulkanLogicalDevice::Release");

    if (m_device != nullptr)
    {
        LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
        VkResult result = vkDeviceWaitIdle(m_device);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("VulkanLogicalDevice: %s", VulkanTypeConverter::ToString(result).c_str());
        }

        m_graphicsCommandPool.reset();
        m_presentCommandPool.reset();
        m_computeCommandPool.reset();
        m_transferCommandPool.reset();

        LOG_TRACE("vkDestroyDevice %s", m_guid.c_str());
        vkDestroyDevice(m_device, nullptr);
        m_device = nullptr;
    }
}

VkDevice VulkanLogicalDevice::GetHandle()
{
    FN("VulkanLogicalDevice::GetHandle");

    return m_device;
}

VulkanPhysicalDevice *VulkanLogicalDevice::GetParentPhysicalDevice()
{
    FN("VulkanLogicalDevice::GetParentPhysicalDevice");

    return m_physicalDevice;
}

VulkanShaderProgramPtr VulkanLogicalDevice::CreateComputeShaderProgram(const std::string &computeShaderSource)
{
    FN("VulkanLogicalDevice::CreateComputeShaderProgram");

    VulkanShaderProgramPtr pComputeShaderProgram = VulkanShaderProgramPtr(new VulkanShaderProgram());
    if (pComputeShaderProgram->Initialize(this, computeShaderSource))
    {
        return pComputeShaderProgram;
    }
    else
    {
        LOG_ERROR("VulkanLogicalDevice: Error while creating Vulkan-ComputeShaderProgram!");
        return VulkanShaderProgramPtr(nullptr);
    }
}

VulkanShaderProgramPtr VulkanLogicalDevice::CreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                                                const std::string &TessEvalShaderSource)
{
    FN("VulkanLogicalDevice::CreateShaderProgram");

    VulkanShaderProgramPtr pShaderProgram = VulkanShaderProgramPtr(new VulkanShaderProgram());
    if (pShaderProgram->Initialize(this, VertexShaderSource, FragmentShaderSource, GeometryShaderSource, TessControlShaderSource, TessEvalShaderSource))
    {
        return pShaderProgram;
    }
    else
    {
        LOG_ERROR("VulkanLogicalDevice: Error while creating Vulkan-ShaderProgram!");
        return VulkanShaderProgramPtr(nullptr);
    }
}

std::unique_ptr<VulkanRayTracingShaderProgram> VulkanLogicalDevice::CreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource,
                                                                                                  const std::string &missShaderSource, const std::string &intersectionShaderSource, const std::string &callableShaderSource)
{
    FN("VulkanLogicalDevice::CreateRayTracingShaderProgram");

    std::unique_ptr<VulkanRayTracingShaderProgram> vulkanRayTracingShaderProgram = std::make_unique<VulkanRayTracingShaderProgram>(this);

    if (vulkanRayTracingShaderProgram->Initialize(rayGenShaderSource, anyHitShaderSource, closestHitShaderSource, missShaderSource, intersectionShaderSource, callableShaderSource))
    {
        return vulkanRayTracingShaderProgram;
    }
    else
    {
        LOG_ERROR("VulkanLogicalDevice: Error while creating Vulkan-RayTracingShaderProgram!");
        return nullptr;
    }
}

VulkanSwapchainPtr VulkanLogicalDevice::CreateSwapchain(VulkanRenderSurface *surface, int width, int height)
{
    FN("VulkanLogicalDevice::CreateSwapchain");

    VulkanSwapchainPtr swapchain = VulkanSwapchainPtr(new VulkanSwapchain(this, surface));
    if (swapchain->Initialize(width, height))
    {
        return swapchain;
    }
    else
    {
        LOG_ERROR("VulkanLogicalDevice: Error while creating Vulkan-Swapchain!");
        return VulkanSwapchainPtr(nullptr);
    }
}

VulkanBufferPtr VulkanLogicalDevice::CreateBuffer(VkBufferUsageFlags usageFlags, int32_t sizeInBytes, VkMemoryPropertyFlags properties)
{
    FN("VulkanLogicalDevice::CreateVertexBuffer");

    VulkanBufferPtr buffer = VulkanBufferPtr(new VulkanBuffer(this));
    buffer->Initialize(sizeInBytes, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | properties);

    return buffer;
}

VulkanVertexBufferPtr VulkanLogicalDevice::CreateVertexBuffer(BufferHint usageHint, int32_t sizeInBytes, bool useForRayTracing)
{
    FN("VulkanLogicalDevice::CreateVertexBuffer");

    VulkanVertexBufferPtr vertexBuffer = VulkanVertexBufferPtr(new VulkanVertexBuffer(this, usageHint, sizeInBytes, useForRayTracing));
    vertexBuffer->Initialize();
    return vertexBuffer;
}

VulkanIndexBufferPtr VulkanLogicalDevice::CreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes, bool useForRayTracing)
{
    FN("VulkanLogicalDevice::CreateIndexBuffer");

    VulkanIndexBufferPtr indexBuffer = VulkanIndexBufferPtr(new VulkanIndexBuffer(this, usageHint, dataType, sizeInBytes, useForRayTracing));
    indexBuffer->Initialize();
    return indexBuffer;
}

VulkanUniformBufferPtr VulkanLogicalDevice::CreateUniformBuffer(BufferHint usageHint, int64_t sizeInBytes)
{
    FN("VulkanLogicalDevice::CreateUniformBuffer");

    VulkanUniformBufferPtr uniformBuffer = VulkanUniformBufferPtr(new VulkanUniformBuffer(this, usageHint, sizeInBytes));
    uniformBuffer->Initialize();
    return uniformBuffer;
}

VulkanStorageBufferPtr VulkanLogicalDevice::CreateStorageBuffer(BufferHint usageHint, int64_t sizeInBytes)
{
    FN("VulkanLogicalDevice::CreateStorageBuffer");

    VulkanStorageBufferPtr storageBuffer = VulkanStorageBufferPtr(new VulkanStorageBuffer(this, usageHint, sizeInBytes));
    storageBuffer->Initialize();
    return storageBuffer;
}

uint32_t VulkanLogicalDevice::GetGraphicsQueueFamilyIndex() const
{
    FN("VulkanLogicalDevice::GetGraphicsQueueFamilyIndex");

    return m_graphicsQueueFamilyIndex;
}

uint32_t VulkanLogicalDevice::GetPresentQueueFamilyIndex() const
{
    FN("VulkanLogicalDevice::GetPresentQueueFamilyIndex");

    return m_presentQueueFamilyIndex;
}

uint32_t VulkanLogicalDevice::GetComputeQueueFamilyIndex() const
{
    FN("VulkanLogicalDevice::GetComputeQueueFamilyIndex");

    return m_computeQueueFamilyIndex;
}

uint32_t VulkanLogicalDevice::GetTransferQueueFamilyIndex() const
{
    FN("VulkanLogicalDevice::GetTransferQueueFamilyIndex");

    return m_transferQueueFamilyIndex;
}

VkQueue VulkanLogicalDevice::GetGraphicsQueue() const
{
    FN("VulkanLogicalDevice::GetGraphicsQueue");

    return m_graphicsQueue;
}

VkQueue VulkanLogicalDevice::GetPresentQueue() const
{
    FN("VulkanLogicalDevice::GetPresentQueue");

    return m_presentQueue;
}

VkQueue VulkanLogicalDevice::GetComputeQueue() const
{
    FN("VulkanLogicalDevice::GetComputeQueue");

    return m_computeQueue;
}

VkQueue VulkanLogicalDevice::GetTransferQueue() const
{
    FN("VulkanLogicalDevice::GetTransferQueue");

    return m_transferQueue;
}

VulkanCommandPoolPtr VulkanLogicalDevice::GetGraphicsCommandPool() const
{
    FN("VulkanLogicalDevice::GetGraphicsCommandPool");

    return m_graphicsCommandPool;
}

VulkanCommandPoolPtr VulkanLogicalDevice::GetPresentCommandPool() const
{
    FN("VulkanLogicalDevice::GetPresentCommandPool");

    return m_presentCommandPool;
}

VulkanCommandPoolPtr VulkanLogicalDevice::GetComputeCommandPool() const
{
    FN("VulkanLogicalDevice::GetComputeCommandPool");

    return m_computeCommandPool;
}

VulkanCommandPoolPtr VulkanLogicalDevice::GetTransferCommandPool() const
{
    FN("VulkanLogicalDevice::GetTransferCommandPool");

    return m_transferCommandPool;
}

VulkanCommandBufferPtr VulkanLogicalDevice::CreateGraphicsCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateGraphicsCommandBuffer");

    VulkanCommandBufferPtr commandBuffer = VulkanCommandBufferPtr(new VulkanCommandBuffer());
    commandBuffer->Allocate(this, m_graphicsCommandPool.get(), 1, true);
    return commandBuffer;
}

VulkanCommandBufferPtr VulkanLogicalDevice::CreatePresentCommandBuffer()
{
    FN("VulkanLogicalDevice::CreatePresentCommandBuffer");

    VulkanCommandBufferPtr commandBuffer = VulkanCommandBufferPtr(new VulkanCommandBuffer());
    commandBuffer->Allocate(this, m_presentCommandPool.get(), 1, true);
    return commandBuffer;
}

VulkanCommandBufferPtr VulkanLogicalDevice::CreateComputeCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateComputeCommandBuffer");

    VulkanCommandBufferPtr commandBuffer = VulkanCommandBufferPtr(new VulkanCommandBuffer());
    commandBuffer->Allocate(this, m_computeCommandPool.get(), 1, true);
    return commandBuffer;
}

VulkanCommandBufferPtr VulkanLogicalDevice::CreateTransferCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateTransferCommandBuffer");

    VulkanCommandBufferPtr commandBuffer = VulkanCommandBufferPtr(new VulkanCommandBuffer());
    commandBuffer->Allocate(this, m_transferCommandPool.get(), 1, true);
    return commandBuffer;
}

VulkanSingleUseCommandBufferPtr VulkanLogicalDevice::CreateSingleUseGraphicsCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateSingleUseGraphicsCommandBuffer");

    VulkanSingleUseCommandBufferPtr commandBuffer = VulkanSingleUseCommandBufferPtr(new VulkanSingleUseCommandBuffer());
    commandBuffer->AllocateAndBegin(this, m_graphicsCommandPool.get(), m_graphicsQueue);
    return commandBuffer;
}

VulkanSingleUseCommandBufferPtr VulkanLogicalDevice::CreateSingleUsePresentCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateSingleUsePresentCommandBuffer");

    VulkanSingleUseCommandBufferPtr commandBuffer = VulkanSingleUseCommandBufferPtr(new VulkanSingleUseCommandBuffer());
    commandBuffer->AllocateAndBegin(this, m_presentCommandPool.get(), m_presentQueue);
    return commandBuffer;
}

VulkanSingleUseCommandBufferPtr VulkanLogicalDevice::CreateSingleUseComputeCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateSingleUseComputeCommandBuffer");

    VulkanSingleUseCommandBufferPtr commandBuffer = VulkanSingleUseCommandBufferPtr(new VulkanSingleUseCommandBuffer());
    commandBuffer->AllocateAndBegin(this, m_computeCommandPool.get(), m_computeQueue);
    return commandBuffer;
}

VulkanSingleUseCommandBufferPtr VulkanLogicalDevice::CreateSingleUseTransferCommandBuffer()
{
    FN("VulkanLogicalDevice::CreateSingleUseTransferCommandBuffer");

    VulkanSingleUseCommandBufferPtr commandBuffer = VulkanSingleUseCommandBufferPtr(new VulkanSingleUseCommandBuffer());
    commandBuffer->AllocateAndBegin(this, m_transferCommandPool.get(), m_transferQueue);
    return commandBuffer;
}

VulkanSemaphorePtr VulkanLogicalDevice::CreateSemaphoreA()
{
    FN("VulkanLogicalDevice::CreateSemaphore");

    VulkanSemaphorePtr semaphore = VulkanSemaphorePtr(new VulkanSemaphore());
    semaphore->Initialize(this);
    return semaphore;
}

VulkanFencePtr VulkanLogicalDevice::CreateFence()
{
    FN("VulkanLogicalDevice::CreateFence");

    VulkanFencePtr fence = VulkanFencePtr(new VulkanFence());
    if (!fence->Initialize(this))
    {
        return nullptr;
    }
    return fence;
}

} // namespace bow
