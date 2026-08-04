#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <VulkanThinDevice/BowVulkanThinObjects.h>

#include <CoreSystems/BowLogger.h>

#include <cstring>

namespace bow
{
namespace
{

//! Routes the validation layers into the engine log.
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT *data, void *)
{
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOG_ERROR("Vulkan: %s", data->pMessage);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOG_WARNING("Vulkan: %s", data->pMessage);
    }
    else
    {
        LOG_TRACE("Vulkan: %s", data->pMessage);
    }
    return VK_FALSE;
}

bool HasLayer(const char *name)
{
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    std::vector<VkLayerProperties> layers(count);
    vkEnumerateInstanceLayerProperties(&count, layers.data());

    for (const VkLayerProperties &layer : layers)
    {
        if (std::strcmp(layer.layerName, name) == 0)
        {
            return true;
        }
    }
    return false;
}

} // namespace

VulkanThinDevice::VulkanThinDevice()
    : m_instance(VK_NULL_HANDLE), m_debugMessenger(VK_NULL_HANDLE), m_physicalDevice(VK_NULL_HANDLE), m_memoryProperties(), m_device(VK_NULL_HANDLE), m_graphicsFamily(UINT32_MAX), m_computeFamily(UINT32_MAX), m_transferFamily(UINT32_MAX),
      m_initialized(false)
{
    FN("VulkanThinDevice::VulkanThinDevice");
}

VulkanThinDevice::~VulkanThinDevice()
{
    FN("VulkanThinDevice::~VulkanThinDevice");

    VRelease();
}

bool VulkanThinDevice::Initialize(uint32_t deviceHandle)
{
    FN("VulkanThinDevice::Initialize");

    if (!CreateInstance() || !PickPhysicalDevice(deviceHandle) || !CreateLogicalDevice())
    {
        return false;
    }

    m_initialized = true;
    return true;
}

bool VulkanThinDevice::CreateInstance()
{
    FN("VulkanThinDevice::CreateInstance");

    VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    applicationInfo.pApplicationName = "LongBow";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(2, 0, 0);
    applicationInfo.pEngineName = "LongBow";
    applicationInfo.engineVersion = VK_MAKE_VERSION(2, 0, 0);
    // Dynamic rendering and synchronisation2 are core in 1.3, and the thin
    // interface is built on both.
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    std::vector<const char *> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
#ifdef _WIN32
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    std::vector<const char *> layers;
#ifdef _DEBUG
    if (HasLayer("VK_LAYER_KHRONOS_validation"))
    {
        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    else
    {
        LOG_WARNING("The Khronos validation layer is not installed; running without it.");
    }
#endif

    VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = (uint32_t)layers.size();
    createInfo.ppEnabledLayerNames = layers.data();

    if (!VulkanCheck(vkCreateInstance(&createInfo, nullptr, &m_instance), "vkCreateInstance"))
    {
        return false;
    }

    if (!layers.empty())
    {
        PFN_vkCreateDebugUtilsMessengerEXT create = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
        if (create != nullptr)
        {
            VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            messengerInfo.pfnUserCallback = DebugCallback;
            create(m_instance, &messengerInfo, nullptr, &m_debugMessenger);
        }
    }

    return true;
}

bool VulkanThinDevice::PickPhysicalDevice(uint32_t deviceHandle)
{
    FN("VulkanThinDevice::PickPhysicalDevice");

    uint32_t count = 0;
    vkEnumeratePhysicalDevices(m_instance, &count, nullptr);
    if (count == 0)
    {
        LOG_ERROR("No Vulkan-capable device found.");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(m_instance, &count, devices.data());

    // deviceHandle names an adapter; anything out of range falls back to
    // preferring a discrete GPU, then to the first one enumerated.
    if (deviceHandle < count)
    {
        m_physicalDevice = devices[deviceHandle];
    }
    else
    {
        m_physicalDevice = devices[0];
        for (VkPhysicalDevice candidate : devices)
        {
            VkPhysicalDeviceProperties properties = {};
            vkGetPhysicalDeviceProperties(candidate, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_physicalDevice = candidate;
                break;
            }
        }
    }

    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);

    m_capabilities.adapterName = properties.deviceName;
    m_capabilities.maxRootConstantBytes = properties.limits.maxPushConstantsSize;

    // Ray tracing is an extension even on hardware that has it, so the
    // capability reflects what this device actually offers.
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> available(extensionCount);
    vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, available.data());

    for (const VkExtensionProperties &extension : available)
    {
        if (std::strcmp(extension.extensionName, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) == 0)
        {
            m_capabilities.rayTracing = true;
        }
    }

    LOG_INFO("Vulkan %u.%u on %s", VK_API_VERSION_MAJOR(properties.apiVersion), VK_API_VERSION_MINOR(properties.apiVersion), properties.deviceName);
    return true;
}

bool VulkanThinDevice::CreateLogicalDevice()
{
    FN("VulkanThinDevice::CreateLogicalDevice");

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &familyCount, families.data());

    // Prefer families that do one thing: a dedicated transfer or compute queue
    // can overlap with graphics work, which a shared one cannot.
    for (uint32_t i = 0; i < familyCount; ++i)
    {
        const VkQueueFlags flags = families[i].queueFlags;

        if ((flags & VK_QUEUE_GRAPHICS_BIT) && m_graphicsFamily == UINT32_MAX)
        {
            m_graphicsFamily = i;
        }
        if ((flags & VK_QUEUE_COMPUTE_BIT) && !(flags & VK_QUEUE_GRAPHICS_BIT))
        {
            m_computeFamily = i;
        }
        if ((flags & VK_QUEUE_TRANSFER_BIT) && !(flags & VK_QUEUE_GRAPHICS_BIT) && !(flags & VK_QUEUE_COMPUTE_BIT))
        {
            m_transferFamily = i;
        }
    }

    if (m_graphicsFamily == UINT32_MAX)
    {
        LOG_ERROR("No graphics queue family on this device.");
        return false;
    }

    if (m_computeFamily == UINT32_MAX)
    {
        m_computeFamily = m_graphicsFamily;
    }
    if (m_transferFamily == UINT32_MAX)
    {
        m_transferFamily = m_graphicsFamily;
    }

    const float priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    std::vector<uint32_t> uniqueFamilies = {m_graphicsFamily};
    if (m_computeFamily != m_graphicsFamily)
    {
        uniqueFamilies.push_back(m_computeFamily);
    }
    if (m_transferFamily != m_graphicsFamily && m_transferFamily != m_computeFamily)
    {
        uniqueFamilies.push_back(m_transferFamily);
    }

    for (uint32_t family : uniqueFamilies)
    {
        VkDeviceQueueCreateInfo queueInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        queueInfo.queueFamilyIndex = family;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &priority;
        queueInfos.push_back(queueInfo);
    }

    std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkPhysicalDeviceVulkan13Features features13 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features12 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features12.timelineSemaphore = VK_TRUE;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.pNext = &features13;

    VkPhysicalDeviceFeatures2 features = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    features.pNext = &features12;
    features.features.samplerAnisotropy = VK_TRUE;
    features.features.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo createInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    createInfo.pNext = &features;
    createInfo.queueCreateInfoCount = (uint32_t)queueInfos.size();
    createInfo.pQueueCreateInfos = queueInfos.data();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    return VulkanCheck(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device), "vkCreateDevice");
}

void VulkanThinDevice::VRelease()
{
    FN("VulkanThinDevice::VRelease");

    if (!m_initialized)
    {
        return;
    }

    vkDeviceWaitIdle(m_device);
    m_queues.clear();

    vkDestroyDevice(m_device, nullptr);
    m_device = VK_NULL_HANDLE;

    if (m_debugMessenger != VK_NULL_HANDLE)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT destroy = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (destroy != nullptr)
        {
            destroy(m_instance, m_debugMessenger, nullptr);
        }
        m_debugMessenger = VK_NULL_HANDLE;
    }

    vkDestroyInstance(m_instance, nullptr);
    m_instance = VK_NULL_HANDLE;

    m_initialized = false;
    LOG_TRACE("VulkanThinDevice released");
}

uint32_t VulkanThinDevice::GetQueueFamily(ThinQueueType type) const
{
    switch (type)
    {
    case ThinQueueType::Compute:
        return m_computeFamily;
    case ThinQueueType::Transfer:
        return m_transferFamily;
    default:
        return m_graphicsFamily;
    }
}

ThinQueuePtr VulkanThinDevice::VGetQueue(ThinQueueType type)
{
    FN("VulkanThinDevice::VGetQueue");

    const uint32_t family = GetQueueFamily(type);
    std::unordered_map<uint32_t, ThinQueuePtr>::const_iterator existing = m_queues.find(family);
    if (existing != m_queues.end())
    {
        return existing->second;
    }

    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(m_device, family, 0, &queue);

    ThinQueuePtr created = std::make_shared<VulkanThinQueue>(this, queue, type);
    m_queues[family] = created;
    return created;
}

VulkanThinDevice::PendingPresentSync VulkanThinDevice::TakePendingPresentSync()
{
    const PendingPresentSync taken = m_pendingPresentSync;
    m_pendingPresentSync = PendingPresentSync();
    return taken;
}

uint32_t VulkanThinDevice::FindMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const
{
    for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; ++i)
    {
        if ((typeBits & (1u << i)) && (m_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    return UINT32_MAX;
}

void VulkanThinDevice::VWaitIdle()
{
    FN("VulkanThinDevice::VWaitIdle");

    vkDeviceWaitIdle(m_device);
}

} // namespace bow
