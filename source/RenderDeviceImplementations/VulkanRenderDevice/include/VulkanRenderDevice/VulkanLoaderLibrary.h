#pragma once
#include <VulkanRenderDevice/VulkanFunctions.h>

#ifdef _WIN32
#include <Windows.h>
#elif defined __linux
#include <dlfcn.h>
#endif

#include <vector>

namespace bow
{

// Vulkan library type
#ifdef _WIN32
#define LIBRARY_TYPE HMODULE
#elif defined __linux
#define LIBRARY_TYPE void *
#endif

// OS-specific parameters
struct WindowParameters
{
#ifdef VK_USE_PLATFORM_WIN32_KHR

    HINSTANCE HInstance;
    HWND HWnd;

#elif defined VK_USE_PLATFORM_XLIB_KHR

    Display *Dpy;
    Window Window;

#elif defined VK_USE_PLATFORM_XCB_KHR

    xcb_connection_t *Connection;
    xcb_window_t Window;

#endif
};

// Extension availability check
bool IsExtensionSupported(std::vector<VkExtensionProperties> const &available_extensions, char const *const extension);

bool ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE &vulkan_library);

bool LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const &vulkan_library);

bool LoadGlobalLevelFunctions();

bool LoadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabled_extensions);

bool LoadDeviceLevelFunctions(VkDevice logical_device, std::vector<char const *> const &enabled_extensions);

} // namespace bow