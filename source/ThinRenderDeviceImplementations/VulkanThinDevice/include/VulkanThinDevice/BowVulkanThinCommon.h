#pragma once
#include <ThinRenderDevice/IBowThinDevice.h>
#include <VulkanThinDevice/VulkanThinDevice_api.h>

#ifdef _WIN32
// Windows.h has to come first: vulkan_win32.h uses HWND and HINSTANCE, and
// VK_USE_PLATFORM_WIN32_KHR is what pulls that header in from vulkan.h.
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#endif

#include <vulkan/vulkan.h>

namespace bow
{

class VulkanThinDevice;

//! Translations between the thin enums and Vulkan.
/*!
    Kept in one place so that the mapping decisions -- particularly which image
    layout and access flags a ThinResourceState means -- are stated once and
    can be read against the DirectX 12 backend's version of the same table.
*/
class VulkanThinTypes
{
  public:
    static VkFormat ToVkFormat(ThinFormat format);
    static ThinFormat FromVkFormat(VkFormat format);

    static VkImageLayout ToImageLayout(ThinResourceState state);
    static VkAccessFlags2 ToAccessFlags(ThinResourceState state);
    static VkPipelineStageFlags2 ToStageFlags(ThinResourceState state);

    static VkShaderStageFlagBits ToShaderStage(ThinShaderStage stage);
    static VkShaderStageFlags ToShaderStageFlags(ThinShaderStage stages);

    static VkDescriptorType ToDescriptorType(ThinDescriptorType type);
    static VkPrimitiveTopology ToPrimitiveTopology(ThinPrimitiveTopology topology);
    static VkCompareOp ToCompareOp(ThinCompareOp op);
    static VkCullModeFlags ToCullMode(ThinCullMode mode);
    static VkBlendFactor ToBlendFactor(ThinBlendFactor factor);
    static VkAttachmentLoadOp ToLoadOp(ThinLoadOp op);
    static VkAttachmentStoreOp ToStoreOp(ThinStoreOp op);

    //! Human-readable form of a VkResult, for log messages.
    static const char *ToString(VkResult result);
};

//! Logs and returns false when a Vulkan call failed.
bool VulkanCheck(VkResult result, const char *what);

} // namespace bow
