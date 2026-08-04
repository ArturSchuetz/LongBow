#include <VulkanRenderDevice/BowVulkanInstance.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/VulkanLoaderLibrary.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bow
{

VulkanInstance::VulkanInstance() : m_vulkanInstance(VK_NULL_HANDLE), m_guid(Utils::GenerateGUID()) { FN("VulkanInstance::VulkanInstance"); }

VulkanInstance::~VulkanInstance()
{
    FN("VulkanInstance::~VulkanInstance");

    Release();
}

bool VulkanInstance::Initialize()
{
    FN("VulkanInstance::Initialize");

    if (m_vulkanInstance != VK_NULL_HANDLE)
    {
        LOG_TRACE("VulkanInstance: Already initialized");
        return true;
    }

    if (glfwInit() == GLFW_FALSE)
    {
        LOG_ERROR("VulkanInstance: Could not initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    uint32_t glfw_extensions_count;
    LOG_TRACE("glfwGetRequiredInstanceExtensions");
    const char **glfw_extensions_names = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

    std::vector<const char *> extensionNames = {};
    for (size_t i = 0; i < glfw_extensions_count; i++)
    {
        extensionNames.push_back(glfw_extensions_names[i]);
    }
#ifdef _DEBUG
    extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    // Note: RT extensions (VK_KHR_ACCELERATION_STRUCTURE, VK_KHR_RAY_TRACING_PIPELINE, etc.)
    // are device-level extensions, not instance-level. They are enabled in BowVulkanLogicalDevice.

    uint32_t pLayersCount = 0;
    LOG_TRACE("vkEnumerateInstanceLayerProperties %s", m_guid.c_str());
    VkResult result = vkEnumerateInstanceLayerProperties(&pLayersCount, nullptr);
    if (result != VK_SUCCESS || pLayersCount == 0)
    {
        LOG_ERROR("VulkanInstance: %s", VulkanTypeConverter::ToString(result).c_str());
        LOG_ERROR("VulkanInstance: No Supported Layers");
        return false; // No layers found or error.
    }

    std::vector<VkLayerProperties> layerProperties(pLayersCount);
    LOG_TRACE("vkEnumerateInstanceLayerProperties %s", m_guid.c_str());
    result = vkEnumerateInstanceLayerProperties(&pLayersCount, &layerProperties[0]);
    if (result != VK_SUCCESS || pLayersCount == 0)
    {
        LOG_ERROR("VulkanInstance: %s", VulkanTypeConverter::ToString(result).c_str());
        LOG_ERROR("VulkanInstance: No Supported Layers");
        return false; // No layers found or error.
    }
    PrintLayerProperties(layerProperties);

    uint32_t pExtensionsCount = 0;
    LOG_TRACE("vkEnumerateInstanceExtensionProperties %s", m_guid.c_str());
    VkResult extension_enum_result = vkEnumerateInstanceExtensionProperties(nullptr, &pExtensionsCount, nullptr);
    if (extension_enum_result != VK_SUCCESS || pExtensionsCount == 0)
    {
        LOG_ERROR("VulkanInstance: %s", VulkanTypeConverter::ToString(result).c_str());
        LOG_ERROR("VulkanInstance: No Supported Extensions");
        return false;
    }

    std::vector<VkExtensionProperties> extensionProperties(pExtensionsCount);
    LOG_TRACE("vkEnumerateInstanceExtensionProperties %s", m_guid.c_str());
    extension_enum_result = vkEnumerateInstanceExtensionProperties(nullptr, &pExtensionsCount, &extensionProperties[0]);
    if (extension_enum_result != VK_SUCCESS || pExtensionsCount == 0)
    {
        LOG_ERROR("VulkanInstance: %s", VulkanTypeConverter::ToString(result).c_str());
        LOG_ERROR("VulkanInstance: No Supported Extensions");
        return false;
    }
    PrintExtensionProperties(extensionProperties);

    // initialize the VkApplicationInfo structure
    VkApplicationInfo applicationInfo = {};
    memset(&applicationInfo, 0, sizeof(applicationInfo));
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "LongBowGame";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    applicationInfo.pEngineName = "LongBow";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    std::vector<const char *> validationLayers = {};
#ifdef _DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    // initialize the VkInstanceCreateInfo structure
    VkInstanceCreateInfo instanceCreateInfo = {};
    memset(&instanceCreateInfo, 0, sizeof(instanceCreateInfo));
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = validationLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    instanceCreateInfo.enabledExtensionCount = extensionNames.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

    LOG_TRACE("vkCreateInstance %s", m_guid.c_str());
    VkResult create_instance_res = vkCreateInstance(&instanceCreateInfo, nullptr, &m_vulkanInstance);
    if (create_instance_res != VK_SUCCESS)
    {
        LOG_ERROR("VulkanInstance: %s", VulkanTypeConverter::ToString(create_instance_res).c_str());
        return false;
    }
    else
    {
        if (!LoadInstanceLevelFunctions(m_vulkanInstance, extensionNames))
        {
            return false;
        }

        return true;
    }
}

void VulkanInstance::Release()
{
    FN("VulkanInstance::Release");

    if (m_vulkanInstance != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyInstance %s", m_guid.c_str());
        vkDestroyInstance(m_vulkanInstance, nullptr);
        m_vulkanInstance = VK_NULL_HANDLE;
    }

    glfwTerminate();
}

VkInstance VulkanInstance::GetHandle()
{
    FN("VulkanInstance::GetHandle");

    return m_vulkanInstance;
}

void VulkanInstance::PrintLayerProperties(const std::vector<VkLayerProperties> &layerProperties)
{
    FN("VulkanInstance::PrintLayerProperties");

    LOG_TRACE("INSTANCE LAYERS:");
    for (size_t l = 0; l < layerProperties.size(); l++)
    {
        LOG_TRACE("\tLayerName: %s", layerProperties[l].layerName);
        LOG_TRACE("\t\tspecVersion: %u", layerProperties[l].specVersion);
        LOG_TRACE("\t\timplementationVersion: %u", layerProperties[l].implementationVersion);
        LOG_TRACE("\t\tdescription: %s", layerProperties[l].description);
    }
}

void VulkanInstance::PrintExtensionProperties(const std::vector<VkExtensionProperties> &extensionProperties)
{
    FN("VulkanInstance::PrintExtensionProperties");

    LOG_TRACE("INSTANCE EXTENSIONS:");
    for (size_t l = 0; l < extensionProperties.size(); l++)
    {
        LOG_TRACE("\tExtensionName: %s", extensionProperties[l].extensionName);
        LOG_TRACE("\t\tspecVersion: %u", extensionProperties[l].specVersion);
    }
}

} // namespace bow