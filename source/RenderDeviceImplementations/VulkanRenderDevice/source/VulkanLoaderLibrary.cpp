#include <VulkanRenderDevice/VulkanLoaderLibrary.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

bool IsExtensionSupported(std::vector<VkExtensionProperties> const &available_extensions, char const *const extension)
{
    for (auto &available_extension : available_extensions)
    {
        if (strstr(available_extension.extensionName, extension))
        {
            return true;
        }
    }
    return false;
}

bool ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE &vulkan_library)
{
#if defined _WIN32
    vulkan_library = LoadLibrary("vulkan-1.dll");
#elif defined __linux
    vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
#endif

    if (vulkan_library == nullptr)
    {
        LOG_ERROR("Could not connect with a Vulkan Runtime library.");
        return false;
    }
    return true;
}

bool LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const &vulkan_library)
{
#if defined _WIN32
#define LoadFunction GetProcAddress
#elif defined __linux
#define LoadFunction dlsym
#endif

#define EXPORTED_VULKAN_FUNCTION(name)                                                                                                                                                                                                                   \
    name = (PFN_##name)LoadFunction(vulkan_library, #name);                                                                                                                                                                                              \
    if (name == nullptr)                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                    \
        LOG_ERROR("Could not load exported Vulkan function named: %s", #name);                                                                                                                                                                           \
        return false;                                                                                                                                                                                                                                    \
    }

#include <VulkanRenderDevice/ListOfVulkanFunctions.inl>

    return true;
}

bool LoadGlobalLevelFunctions()
{
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                                                                                                                                                                                               \
    name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);                                                                                                                                                                                            \
    if (name == nullptr)                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                    \
        LOG_ERROR("Could not load global level Vulkan function named: %s", #name);                                                                                                                                                                       \
        return false;                                                                                                                                                                                                                                    \
    }

#include <VulkanRenderDevice/ListOfVulkanFunctions.inl>

    return true;
}

bool LoadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabled_extensions)
{
    // Load core Vulkan API instance-level functions
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                                                                                                                                                                                             \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                                                                                                                                                                                           \
    if (name == nullptr)                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                    \
        LOG_ERROR("Could not load instance-level Vulkan function named: %s", #name);                                                                                                                                                                     \
        return false;                                                                                                                                                                                                                                    \
    }

    // Load instance-level functions from enabled extensions
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                                                                                                                                                                                   \
    for (auto &enabled_extension : enabled_extensions)                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                    \
        if (std::string(enabled_extension) == std::string(extension))                                                                                                                                                                                    \
        {                                                                                                                                                                                                                                                \
            name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                                                                                                                                                                                   \
            if (name == nullptr)                                                                                                                                                                                                                         \
            {                                                                                                                                                                                                                                            \
                LOG_ERROR("Could not load instance-level Vulkan function named: ", #name);                                                                                                                                                               \
                return false;                                                                                                                                                                                                                            \
            }                                                                                                                                                                                                                                            \
        }                                                                                                                                                                                                                                                \
    }

#include <VulkanRenderDevice/ListOfVulkanFunctions.inl>

    return true;
}

bool LoadDeviceLevelFunctions(VkDevice logical_device, std::vector<char const *> const &enabled_extensions)
{
    // Load core Vulkan API device-level functions
#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                                                                                                                                                                                               \
    name = (PFN_##name)vkGetDeviceProcAddr(logical_device, #name);                                                                                                                                                                                       \
    if (name == nullptr)                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                    \
        LOG_ERROR("Could not load device-level Vulkan function named: " #name);                                                                                                                                                                          \
        return false;                                                                                                                                                                                                                                    \
    }

    // Load device-level functions from enabled extensions
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                                                                                                                                                                                     \
    for (auto &enabled_extension : enabled_extensions)                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                    \
        if (std::string(enabled_extension) == std::string(extension))                                                                                                                                                                                    \
        {                                                                                                                                                                                                                                                \
            name = (PFN_##name)vkGetDeviceProcAddr(logical_device, #name);                                                                                                                                                                               \
            if (name == nullptr)                                                                                                                                                                                                                         \
            {                                                                                                                                                                                                                                            \
                LOG_ERROR("Could not load device-level Vulkan function named: " #name);                                                                                                                                                                  \
                return false;                                                                                                                                                                                                                            \
            }                                                                                                                                                                                                                                            \
        }                                                                                                                                                                                                                                                \
    }

#include <VulkanRenderDevice/ListOfVulkanFunctions.inl>

    return true;
}

} // namespace bow
