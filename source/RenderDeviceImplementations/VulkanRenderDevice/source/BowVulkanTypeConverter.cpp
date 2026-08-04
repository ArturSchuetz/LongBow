#include <VulkanRenderDevice/BowVulkanTypeConverter.h>

#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/Buffer/BowBufferHint.h>
#include <RenderDevice/Device/Buffer/BowIndexBufferDatatype.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>
#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace bow
{

std::string VulkanTypeConverter::ToString(VkResult result)
{
    FN("VulkanTypeConverter::ToString");

    switch (result)
    {
    case VK_SUCCESS:
        return "VK_SUCCESS: Command successfully completed.";
    case VK_NOT_READY:
        return "VK_NOT_READY: A fence or query has not yet completed.";
    case VK_TIMEOUT:
        return "VK_TIMEOUT: A wait operation has not completed in the specified time.";
    case VK_EVENT_SET:
        return "VK_EVENT_SET: An event is signaled.";
    case VK_EVENT_RESET:
        return "VK_EVENT_RESET: An event is unsignaled.";
    case VK_INCOMPLETE:
        return "VK_INCOMPLETE: A return array was too small for the result.";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY: A host memory allocation has failed.";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY: A device memory allocation has failed.";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED: Initialization of an object could not be completed for implementation-specific reasons.";
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST: The logical or physical device has been lost.";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED: Mapping of a memory object has failed.";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT: A requested layer is not present or could not be loaded.";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT: A requested extension is not supported.";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT: A requested feature is not supported.";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER: The requested version of Vulkan is not supported by the driver or is otherwise incompatible.";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS: Too many objects of the type have already been created.";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED: A requested format is not supported on this device.";
    case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL: A pool allocation has failed due to fragmentation of the pool's memory.";
    case VK_ERROR_UNKNOWN:
        return "VK_ERROR_UNKNOWN: An unknown error has occurred.";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY: A pool memory allocation has failed.";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE: An external handle is not a valid handle of the specified type.";
    case VK_ERROR_FRAGMENTATION:
        return "VK_ERROR_FRAGMENTATION: A descriptor pool creation has failed due to fragmentation.";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: A buffer creation failed because the requested address is not available.";
    case VK_PIPELINE_COMPILE_REQUIRED:
        return "VK_PIPELINE_COMPILE_REQUIRED: A requested pipeline creation requires compilation, but the application requested compilation to be deferred.";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR: A surface is no longer available.";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: The requested window is already connected to a Vulkan surface.";
    case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR: A surface has changed in such a way that it is no longer compatible with the swapchain.";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: The display used by a swapchain does not use the same presentable image layout.";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "VK_ERROR_VALIDATION_FAILED_EXT: A validation layer found an error.";
    case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV: One or more shaders failed to compile or link.";
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: The requested image usage flags are not supported for this image format.";
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: The requested video picture layout is not supported.";
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: The requested video profile operation is not supported.";
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: The requested video profile format is not supported.";
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: The requested video profile codec is not supported.";
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: The requested video standard version is not supported.";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: The DRM format modifier plane layout is invalid.";
    case VK_ERROR_NOT_PERMITTED_KHR:
        return "VK_ERROR_NOT_PERMITTED_KHR: The operation is not permitted.";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: Full-screen exclusive mode was lost.";
    case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR: A thread is idle.";
    case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR: A thread is done executing.";
    case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR: An operation was deferred.";
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR: An operation was not deferred.";
    case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
        return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: The video standard parameters are invalid.";
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT: Compression exhausted.";
    case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
        return "VK_INCOMPATIBLE_SHADER_BINARY_EXT: The shader binary is incompatible.";
    default:
        return "UNKNOWN: An unknown error has occurred.";
    }
}

std::string VulkanTypeConverter::ToString(VkFormat format)
{
    FN("VulkanTypeConverter::ToString");

    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return "VK_FORMAT_UNDEFINED: An undefined format.";
    case VK_FORMAT_R4G4_UNORM_PACK8:
        return "VK_FORMAT_R4G4_UNORM_PACK8: A two-component, 4-bit unsigned normalized format that has a 4-bit R component in the least significant bits and a 4-bit G component in the most significant bits.";
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        return "VK_FORMAT_R4G4B4A4_UNORM_PACK16: A four-component, 4-bit unsigned normalized format that has a 4-bit R component in the least significant bits and a 4-bit A component in the most significant bits.";
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
        return "VK_FORMAT_B4G4R4A4_UNORM_PACK16: A four-component, 4-bit unsigned normalized format that has a 4-bit B component in the least significant bits and a 4-bit A component in the most significant bits.";
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return "VK_FORMAT_R5G6B5_UNORM_PACK16: A three-component, 5-6-5-bit unsigned normalized format that has a 5-bit R component in the most significant bits, a 6-bit G component, and a 5-bit B component in the least significant bits.";
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
        return "VK_FORMAT_B5G6R5_UNORM_PACK16: A three-component, 5-6-5-bit unsigned normalized format that has a 5-bit B component in the most significant bits, a 6-bit G component, and a 5-bit R component in the least significant bits.";
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
        return "VK_FORMAT_R5G5B5A1_UNORM_PACK16: A four-component, 5-5-5-1-bit unsigned normalized format that has a 5-bit R component in the most significant bits, a 5-bit G component, a 5-bit B component, and a 1-bit A component in the least "
               "significant bits.";
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
        return "VK_FORMAT_B5G5R5A1_UNORM_PACK16: A four-component, 5-5-5-1-bit unsigned normalized format that has a 5-bit B component in the most significant bits, a 5-bit G component, a 5-bit R component, and a 1-bit A component in the least "
               "significant bits.";
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        return "VK_FORMAT_A1R5G5B5_UNORM_PACK16: A four-component, 5-5-5-1-bit unsigned normalized format that has a 1-bit A component in the most significant bits, a 5-bit R component, a 5-bit G component, and a 5-bit B component in the least "
               "significant bits.";
    case VK_FORMAT_R8_UNORM:
        return "VK_FORMAT_R8_UNORM: A one-component, 8-bit unsigned normalized format that has a single 8-bit R component.";
    case VK_FORMAT_R8_SNORM:
        return "VK_FORMAT_R8_SNORM: A one-component, 8-bit signed normalized format that has a single 8-bit R component.";
    case VK_FORMAT_R8_USCALED:
        return "VK_FORMAT_R8_USCALED: A one-component, 8-bit unsigned scaled integer format that has a single 8-bit R component.";
    case VK_FORMAT_R8_SSCALED:
        return "VK_FORMAT_R8_SSCALED: A one-component, 8-bit signed scaled integer format that has a single 8-bit R component.";
    case VK_FORMAT_R8_UINT:
        return "VK_FORMAT_R8_UINT: A one-component, 8-bit unsigned integer format that has a single 8-bit R component.";
    case VK_FORMAT_R8_SINT:
        return "VK_FORMAT_R8_SINT: A one-component, 8-bit signed integer format that has a single 8-bit R component.";
    case VK_FORMAT_R8_SRGB:
        return "VK_FORMAT_R8_SRGB: A one-component, 8-bit sRGB format that has a single 8-bit R component.";
    case VK_FORMAT_R8G8_UNORM:
        return "VK_FORMAT_R8G8_UNORM: A two-component, 8-bit unsigned normalized format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8_SNORM:
        return "VK_FORMAT_R8G8_SNORM: A two-component, 8-bit signed normalized format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8_USCALED:
        return "VK_FORMAT_R8G8_USCALED: A two-component, 8-bit unsigned scaled integer format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8_SSCALED:
        return "VK_FORMAT_R8G8_SSCALED: A two-component, 8-bit signed scaled integer format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8_UINT:
        return "VK_FORMAT_R8G8_UINT: A two-component, 8-bit unsigned integer format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8_SINT:
        return "VK_FORMAT_R8G8_SINT: A two-component, 8-bit signed integer format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8_SRGB:
        return "VK_FORMAT_R8G8_SRGB: A two-component, 8-bit sRGB format that has an 8-bit R component in the first 8 bits and an 8-bit G component in the second 8 bits.";
    case VK_FORMAT_R8G8B8_UNORM:
        return "VK_FORMAT_R8G8B8_UNORM: A three-component, 8-bit unsigned normalized format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_R8G8B8_SNORM:
        return "VK_FORMAT_R8G8B8_SNORM: A three-component, 8-bit signed normalized format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_R8G8B8_USCALED:
        return "VK_FORMAT_R8G8B8_USCALED: A three-component, 8-bit unsigned scaled integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_R8G8B8_SSCALED:
        return "VK_FORMAT_R8G8B8_SSCALED: A three-component, 8-bit signed scaled integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_R8G8B8_UINT:
        return "VK_FORMAT_R8G8B8_UINT: A three-component, 8-bit unsigned integer format that has an 8-bit R component in the first 8 bits, an 8 bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_R8G8B8_SINT:
        return "VK_FORMAT_R8G8B8_SINT: A three-component, 8-bit signed integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_R8G8B8_SRGB:
        return "VK_FORMAT_R8G8B8_SRGB: A three-component, 8-bit sRGB format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit B component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_UNORM:
        return "VK_FORMAT_B8G8R8_UNORM: A three-component, 8-bit unsigned normalized format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_SNORM:
        return "VK_FORMAT_B8G8R8_SNORM: A three-component, 8-bit signed normalized format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_USCALED:
        return "VK_FORMAT_B8G8R8_USCALED: A three-component, 8-bit unsigned scaled integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_SSCALED:
        return "VK_FORMAT_B8G8R8_SSCALED: A three-component, 8-bit signed scaled integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_UINT:
        return "VK_FORMAT_B8G8R8_UINT: A three-component, 8-bit unsigned integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_SINT:
        return "VK_FORMAT_B8G8R8_SINT: A three-component, 8-bit signed integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_B8G8R8_SRGB:
        return "VK_FORMAT_B8G8R8_SRGB: A three-component, 8-bit sRGB format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, and an 8-bit R component in the third 8 bits.";
    case VK_FORMAT_R8G8B8A8_UNORM:
        return "VK_FORMAT_R8G8B8A8_UNORM: A four-component, 8-bit unsigned normalized format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_R8G8B8A8_SNORM:
        return "VK_FORMAT_R8G8B8A8_SNORM: A four-component, 8-bit signed normalized format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_R8G8B8A8_USCALED:
        return "VK_FORMAT_R8G8B8A8_USCALED: A four-component, 8-bit unsigned scaled integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit "
               "A component in the fourth 8 bits.";
    case VK_FORMAT_R8G8B8A8_SSCALED:
        return "VK_FORMAT_R8G8B8A8_SSCALED: A four-component, 8-bit signed scaled integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_R8G8B8A8_UINT:
        return "VK_FORMAT_R8G8B8A8_UINT: A four-component, 8-bit unsigned integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_R8G8B8A8_SINT:
        return "VK_FORMAT_R8G8B8A8_SINT: A four-component, 8-bit signed integer format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit A component "
               "in the fourth 8 bits.";
    case VK_FORMAT_R8G8B8A8_SRGB:
        return "VK_FORMAT_R8G8B8A8_SRGB: A four-component, 8-bit sRGB format that has an 8-bit R component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit B component in the third 8 bits, and an 8-bit A component in the "
               "fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_UNORM:
        return "VK_FORMAT_B8G8R8A8_UNORM: A four-component, 8-bit unsigned normalized format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_SNORM:
        return "VK_FORMAT_B8G8R8A8_SNORM: A four-component, 8-bit signed normalized format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_USCALED:
        return "VK_FORMAT_B8G8R8A8_USCALED: A four-component, 8-bit unsigned scaled integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit "
               "A component in the fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_SSCALED:
        return "VK_FORMAT_B8G8R8A8_SSCALED: A four-component, 8-bit signed scaled integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_UINT:
        return "VK_FORMAT_B8G8R8A8_UINT: A four-component, 8-bit unsigned integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit A "
               "component in the fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_SINT:
        return "VK_FORMAT_B8G8R8A8_SINT: A four-component, 8-bit signed integer format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit A component "
               "in the fourth 8 bits.";
    case VK_FORMAT_B8G8R8A8_SRGB:
        return "VK_FORMAT_B8G8R8A8_SRGB: A four-component, 8-bit sRGB format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the second 8 bits, an 8-bit R component in the third 8 bits, and an 8-bit A component in the "
               "fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        return "VK_FORMAT_A8B8G8R8_UNORM_PACK32: A four-component, 8-bit unsigned normalized format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an 8-bit "
               "R component in the fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
        return "VK_FORMAT_A8B8G8R8_SNORM_PACK32: A four-component, 8-bit signed normalized format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an 8-bit R "
               "component in the fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
        return "VK_FORMAT_A8B8G8R8_USCALED_PACK32: A four-component, 8-bit unsigned scaled integer format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an "
               "8-bit R component in the fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
        return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32: A four-component, 8-bit signed scaled integer format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an "
               "8-bit R component in the fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
        return "VK_FORMAT_A8B8G8R8_UINT_PACK32: A four-component, 8-bit unsigned integer format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an 8-bit R "
               "component in the fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
        return "VK_FORMAT_A8B8G8R8_SINT_PACK32: A four-component, 8-bit signed integer format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an 8-bit R "
               "component in the fourth 8 bits.";
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        return "VK_FORMAT_A8B8G8R8_SRGB_PACK32: A four-component, 8-bit sRGB format that has an 8-bit A component in the first 8 bits, an 8-bit B component in the second 8 bits, an 8-bit G component in the third 8 bits, and an 8-bit R component in "
               "the fourth 8 bits.";
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_UNORM_PACK32: A four-component, 2-10-10-10-bit unsigned normalized format that has a 2-bit A component in the most significant bits, a 10-bit R component, a 10-bit G component, and a 10-bit B component in the "
               "least significant bits.";
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_SNORM_PACK32: A four-component, 2-10-10-10-bit signed normalized format that has a 2-bit A component in the most significant bits, a 10-bit R component, a 10-bit G component, and a 10-bit B component in the "
               "least significant bits.";
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
        return "VK_FORMAT_A2R10G10B10_USCALED_PACK32: A four-component, 2-10-10-10-bit unsigned scaled integer format that has a 2-bit A component in the most significant bits, a 10-bit R component, a 10-bit G component, and a 10-bit B component in "
               "the least significant bits.";
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
        return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32: A four-component, 2-10-10-10-bit signed scaled integer format that has a 2-bit A component in the most significant bits, a 10-bit R component, a 10-bit G component, and a 10-bit B component in "
               "the least significant bits.";
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
        return "VK_FORMAT_A2R10G10B10_UINT_PACK32: A four-component, 2-10-10-10-bit unsigned integer format that has a 2-bit A component in the most significant bits, a 10-bit R component, a 10-bit G component, and a 10-bit B component in the least "
               "significant bits.";
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
        return "VK_FORMAT_A2R10G10B10_SINT_PACK32: A four-component, 2-10-10-10-bit signed integer format that has a 2-bit A component in the most significant bits, a 10-bit R component, a 10-bit G component, and a 10-bit B component in the least "
               "significant bits.";
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_UNORM_PACK32: A four-component, 2-10-10-10-bit unsigned normalized format that has a 2-bit A component in the most significant bits, a 10-bit B component, a 10-bit G component, and a 10-bit R component in the "
               "least significant bits.";
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_SNORM_PACK32: A four-component, 2-10-10-10-bit signed normalized format that has a 2-bit A component in the most significant bits, a 10-bit B component, a 10-bit G component, and a 10-bit R component in the "
               "least significant bits.";
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
        return "VK_FORMAT_A2B10G10R10_USCALED_PACK32: A four-component, 2-10-10-10-bit unsigned scaled integer format that has a 2-bit A component in the most significant bits, a 10-bit B component, a 10-bit G component, and a 10-bit R component in "
               "the least significant bits.";
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
        return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32: A four-component, 2-10-10-10-bit signed scaled integer format that has a 2-bit A component in the most significant bits, a 10-bit B component, a 10-bit G component, and a 10-bit R component in "
               "the least significant bits.";
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
        return "VK_FORMAT_A2B10G10R10_UINT_PACK32: A four-component, 2-10-10-10-bit unsigned integer format that has a 2-bit A component in the most significant bits, a 10-bit B component, a 10-bit G component, and a 10-bit R component in the least "
               "significant bits.";
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
        return "VK_FORMAT_A2B10G10R10_SINT_PACK32: A four-component, 2-10-10-10-bit signed integer format that has a 2-bit A component in the most significant bits, a 10-bit B component, a 10-bit G component, and a 10-bit R component in the least "
               "significant bits.";
    case VK_FORMAT_R16_UNORM:
        return "VK_FORMAT_R16_UNORM: A one-component, 16-bit unsigned normalized format that has a single 16-bit R component.";
    case VK_FORMAT_R16_SNORM:
        return "VK_FORMAT_R16_SNORM: A one-component, 16-bit signed normalized format that has a single 16-bit R component.";
    case VK_FORMAT_R16_USCALED:
        return "VK_FORMAT_R16_USCALED: A one-component, 16-bit unsigned scaled integer format that has a single 16-bit R component.";
    case VK_FORMAT_R16_SSCALED:
        return "VK_FORMAT_R16_SSCALED: A one-component, 16-bit signed scaled integer format that has a single 16-bit R component.";
    case VK_FORMAT_R16_UINT:
        return "VK_FORMAT_R16_UINT: A one-component, 16-bit unsigned integer format that has a single 16-bit R component.";
    case VK_FORMAT_R16_SINT:
        return "VK_FORMAT_R16_SINT: A one-component, 16-bit signed integer format that has a single 16-bit R component.";
    case VK_FORMAT_R16_SFLOAT:
        return "VK_FORMAT_R16_SFLOAT: A one-component, 16-bit floating-point format that has a single 16-bit R component.";
    case VK_FORMAT_R16G16_UNORM:
        return "VK_FORMAT_R16G16_UNORM: A two-component, 16-bit unsigned normalized format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16_SNORM:
        return "VK_FORMAT_R16G16_SNORM: A two-component, 16-bit signed normalized format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16_USCALED:
        return "VK_FORMAT_R16G16_USCALED: A two-component, 16-bit unsigned scaled integer format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16_SSCALED:
        return "VK_FORMAT_R16G16_SSCALED: A two-component, 16-bit signed scaled integer format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16_UINT:
        return "VK_FORMAT_R16G16_UINT: A two-component, 16-bit unsigned integer format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16_SINT:
        return "VK_FORMAT_R16G16_SINT: A two-component, 16-bit signed integer format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16_SFLOAT:
        return "VK_FORMAT_R16G16_SFLOAT: A two-component, 16-bit floating-point format that has a 16-bit R component in the first 16 bits and a 16-bit G component in the second 16 bits.";
    case VK_FORMAT_R16G16B16_UNORM:
        return "VK_FORMAT_R16G16B16_UNORM: A three-component, 16-bit unsigned normalized format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16_SNORM:
        return "VK_FORMAT_R16G16B16_SNORM: A three-component, 16-bit signed normalized format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16_USCALED:
        return "VK_FORMAT_R16G16B16_USCALED: A three-component, 16-bit unsigned scaled integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16_SSCALED:
        return "VK_FORMAT_R16G16B16_SSCALED: A three-component, 16-bit signed scaled integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16_UINT:
        return "VK_FORMAT_R16G16B16_UINT: A three-component, 16-bit unsigned integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16_SINT:
        return "VK_FORMAT_R16G16B16_SINT: A three-component, 16-bit signed integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16_SFLOAT:
        return "VK_FORMAT_R16G16B16_SFLOAT: A three-component, 16-bit floating-point format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, and a 16-bit B component in the third 16 bits.";
    case VK_FORMAT_R16G16B16A16_UNORM:
        return "VK_FORMAT_R16G16B16A16_UNORM: A four-component, 16-bit unsigned normalized format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a "
               "16-bit A component in the fourth 16 bits.";
    case VK_FORMAT_R16G16B16A16_SNORM:
        return "VK_FORMAT_R16G16B16A16_SNORM: A four-component, 16-bit signed normalized format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a 16-bit "
               "A component in the fourth 16 bits.";
    case VK_FORMAT_R16G16B16A16_USCALED:
        return "VK_FORMAT_R16G16B16A16_USCALED: A four-component, 16-bit unsigned scaled integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a "
               "16-bit A component in the fourth 16 bits.";
    case VK_FORMAT_R16G16B16A16_SSCALED:
        return "VK_FORMAT_R16G16B16A16_SSCALED: A four-component, 16-bit signed scaled integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a "
               "16-bit A component in the fourth 16 bits.";
    case VK_FORMAT_R16G16B16A16_UINT:
        return "VK_FORMAT_R16G16B16A16_UINT: A four-component, 16-bit unsigned integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a 16-bit A "
               "component in the fourth 16 bits.";
    case VK_FORMAT_R16G16B16A16_SINT:
        return "VK_FORMAT_R16G16B16A16_SINT: A four-component, 16-bit signed integer format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a 16-bit A "
               "component in the fourth 16 bits.";
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return "VK_FORMAT_R16G16B16A16_SFLOAT: A four-component, 16-bit floating-point format that has a 16-bit R component in the first 16 bits, a 16-bit G component in the second 16 bits, a 16-bit B component in the third 16 bits, and a 16-bit A "
               "component in the fourth 16 bits.";
    case VK_FORMAT_R32_UINT:
        return "VK_FORMAT_R32_UINT: A one-component, 32-bit unsigned integer format that has a single 32-bit R component.";
    case VK_FORMAT_R32_SINT:
        return "VK_FORMAT_R32_SINT: A one-component, 32-bit signed integer format that has a single 32-bit R component.";
    case VK_FORMAT_R32_SFLOAT:
        return "VK_FORMAT_R32_SFLOAT: A one-component, 32-bit floating-point format that has a single 32-bit R component.";
    case VK_FORMAT_R32G32_UINT:
        return "VK_FORMAT_R32G32_UINT: A two-component, 32-bit unsigned integer format that has a 32-bit R component in the first 32 bits and a 32-bit G component in the second 32 bits.";
    case VK_FORMAT_R32G32_SINT:
        return "VK_FORMAT_R32G32_SINT: A two-component, 32-bit signed integer format that has a 32-bit R component in the first 32 bits and a 32-bit G component in the second 32 bits.";
    case VK_FORMAT_R32G32_SFLOAT:
        return "VK_FORMAT_R32G32_SFLOAT: A two-component, 32-bit floating-point format that has a 32-bit R component in the first 32 bits and a 32-bit G component in the second 32 bits.";
    case VK_FORMAT_R32G32B32_UINT:
        return "VK_FORMAT_R32G32B32_UINT: A three-component, 32-bit unsigned integer format that has a 32-bit R component in the first 32 bits, a 32-bit G component in the second 32 bits, and a 32-bit B component in the third 32 bits.";
    case VK_FORMAT_R32G32B32_SINT:
        return "VK_FORMAT_R32G32B32_SINT: A three-component, 32-bit signed integer format that has a 32-bit R component in the first 32 bits, a 32-bit G component in the second 32 bits, and a 32-bit B component in the third 32 bits.";
    case VK_FORMAT_R32G32B32_SFLOAT:
        return "VK_FORMAT_R32G32B32_SFLOAT: A three-component, 32-bit floating-point format that has a 32-bit R component in the first 32 bits, a 32-bit G component in the second 32 bits, and a 32-bit B component in the third 32 bits.";
    case VK_FORMAT_R32G32B32A32_UINT:
        return "VK_FORMAT_R32G32B32A32_UINT: A four-component, 32-bit unsigned integer format that has a 32-bit R component in the first 32 bits, a 32-bit G component in the second 32 bits, a 32-bit B component in the third 32 bits, and a 32-bit A "
               "component in the fourth 32 bits.";
    case VK_FORMAT_R32G32B32A32_SINT:
        return "VK_FORMAT_R32G32B32A32_SINT: A four-component, 32-bit signed integer format that has a 32-bit R component in the first 32 bits, a 32-bit G component in the second 32 bits, a 32-bit B component in the third 32 bits, and a 32-bit A "
               "component in the fourth 32 bits.";
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return "VK_FORMAT_R32G32B32A32_SFLOAT: A four-component, 32-bit floating-point format that has a 32-bit R component in the first 32 bits, a 32-bit G component in the second 32 bits, a 32-bit B component in the third 32 bits, and a 32-bit A "
               "component in the fourth 32 bits.";
    case VK_FORMAT_R64_UINT:
        return "VK_FORMAT_R64_UINT: A one-component, 64-bit unsigned integer format that has a single 64-bit R component.";
    case VK_FORMAT_R64_SINT:
        return "VK_FORMAT_R64_SINT: A one-component, 64-bit signed integer format that has a single 64-bit R component.";
    case VK_FORMAT_R64_SFLOAT:
        return "VK_FORMAT_R64_SFLOAT: A one-component, 64-bit floating-point format that has a single 64-bit R component.";
    case VK_FORMAT_R64G64_UINT:
        return "VK_FORMAT_R64G64_UINT: A two-component, 64-bit unsigned integer format that has a 64-bit R component in the first 64 bits and a 64-bit G component in the second 64 bits.";
    case VK_FORMAT_R64G64_SINT:
        return "VK_FORMAT_R64G64_SINT: A two-component, 64-bit signed integer format that has a 64-bit R component in the first 64 bits and a 64-bit G component in the second 64 bits.";
    case VK_FORMAT_R64G64_SFLOAT:
        return "VK_FORMAT_R64G64_SFLOAT: A two-component, 64-bit floating-point format that has a 64-bit R component in the first 64 bits and a 64-bit G component in the second 64 bits.";
    case VK_FORMAT_R64G64B64_UINT:
        return "VK_FORMAT_R64G64B64_UINT: A three-component, 64-bit unsigned integer format that has a 64-bit R component in the first 64 bits, a 64-bit G component in the second 64 bits, and a 64-bit B component in the third 64 bits.";
    case VK_FORMAT_R64G64B64_SINT:
        return "VK_FORMAT_R64G64B64_SINT: A three-component, 64-bit signed integer format that has a 64-bit R component in the first 64 bits, a 64-bit G component in the second 64 bits, and a 64-bit B component in the third 64 bits.";
    case VK_FORMAT_R64G64B64_SFLOAT:
        return "VK_FORMAT_R64G64B64_SFLOAT: A three-component, 64-bit floating-point format that has a 64-bit R component in the first 64 bits, a 64-bit G component in the second 64 bits, and a 64-bit B component in the third 64 bits.";
    case VK_FORMAT_R64G64B64A64_UINT:
        return "VK_FORMAT_R64G64B64A64_UINT: A four-component, 64-bit unsigned integer format that has a 64-bit R component in the first 64 bits, a 64-bit G component in the second 64 bits, a 64-bit B component in the third 64 bits, and a "
               "64-bit A component in the fourth 64 bits.";
    case VK_FORMAT_R64G64B64A64_SINT:
        return "VK_FORMAT_R64G64B64A64_SINT: A four-component, 64-bit signed integer format that has a 64-bit R component in the first 64 bits, a 64-bit G component in the second 64 bits, a 64-bit B component in the third 64 bits, and a "
               "64-bit A component in the fourth 64 bits.";
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return "VK_FORMAT_R64G64B64A64_SFLOAT: A four-component, 64-bit floating-point format that has a 64-bit R component in the first 64 bits, a 64-bit G component in the second 64 bits, a 64-bit B component in the third 64 bits, and a "
               "64-bit A component in the fourth 64 bits.";
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        return "VK_FORMAT_B10G11R11_UFLOAT_PACK32: A three-component, 10-11-11-bit unsigned floating-point format that has a 10-bit B component in the first 10 bits, an 11-bit G component in the next 11 bits, and an 11-bit R "
               "component in the last 11 bits.";
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
        return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: A three-component, 5-9-9-bit unsigned floating-point format that has a 5-bit E component in the first 5 bits, a 9-bit B component in the next 9 bits, a 9-bit G component in the "
               "next 9 bits, and a 9-bit R component in the last 9 bits.";
    case VK_FORMAT_D16_UNORM:
        return "VK_FORMAT_D16_UNORM: A one-component, 16-bit unsigned normalized format that has a single 16-bit depth component.";
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return "VK_FORMAT_X8_D24_UNORM_PACK32: A two-component, 32-bit unsigned normalized format that has an 8-bit stencil component in the first 8 bits and a 24-bit depth component in the last 24 bits.";
    case VK_FORMAT_D32_SFLOAT:
        return "VK_FORMAT_D32_SFLOAT: A one-component, 32-bit floating-point format that has a single 32-bit depth component.";
    case VK_FORMAT_S8_UINT:
        return "VK_FORMAT_S8_UINT: A one-component, 8-bit unsigned integer format that has a single 8-bit stencil component.";
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return "VK_FORMAT_D16_UNORM_S8_UINT: A two-component format that has a 16-bit unsigned normalized depth component in the first 16 bits and an 8-bit unsigned integer stencil component in the last 8 bits.";
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return "VK_FORMAT_D24_UNORM_S8_UINT: A two-component format that has a 24-bit unsigned normalized depth component in the first 24 bits and an 8-bit unsigned integer stencil component in the last 8 bits.";
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return "VK_FORMAT_D32_SFLOAT_S8_UINT: A two-component format that has a 32-bit floating-point depth component in the first 32 bits and an 8-bit unsigned integer stencil component in the last 8 bits.";
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        return "VK_FORMAT_BC1_RGB_UNORM_BLOCK: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGB texel data.";
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        return "VK_FORMAT_BC1_RGB_SRGB_BLOCK: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGB texel data.";
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_BC2_UNORM_BLOCK:
        return "VK_FORMAT_BC2_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data with separate alpha data.";
    case VK_FORMAT_BC2_SRGB_BLOCK:
        return "VK_FORMAT_BC2_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data with separate alpha data.";
    case VK_FORMAT_BC3_UNORM_BLOCK:
        return "VK_FORMAT_BC3_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data with interpolated alpha data.";
    case VK_FORMAT_BC3_SRGB_BLOCK:
        return "VK_FORMAT_BC3_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data with interpolated alpha data.";
    case VK_FORMAT_BC4_UNORM_BLOCK:
        return "VK_FORMAT_BC4_UNORM_BLOCK: A one-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized red texel data.";
    case VK_FORMAT_BC4_SNORM_BLOCK:
        return "VK_FORMAT_BC4_SNORM_BLOCK: A one-component format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of signed normalized red texel data.";
    case VK_FORMAT_BC5_UNORM_BLOCK:
        return "VK_FORMAT_BC5_UNORM_BLOCK: A two-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized red and green texel data.";
    case VK_FORMAT_BC5_SNORM_BLOCK:
        return "VK_FORMAT_BC5_SNORM_BLOCK: A two-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of signed normalized red and green texel data.";
    case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        return "VK_FORMAT_BC6H_UFLOAT_BLOCK: A three-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned floating-point RGB texel data.";
    case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        return "VK_FORMAT_BC6H_SFLOAT_BLOCK: A three-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of signed floating-point RGB texel data.";
    case VK_FORMAT_BC7_UNORM_BLOCK:
        return "VK_FORMAT_BC7_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_BC7_SRGB_BLOCK:
        return "VK_FORMAT_BC7_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: A three-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGB texel data.";
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: A three-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGB texel data.";
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGB texel data with 1-bit alpha.";
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGB texel data with 1-bit alpha.";
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_EAC_R11_UNORM_BLOCK:
        return "VK_FORMAT_EAC_R11_UNORM_BLOCK: A one-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized red texel data.";
    case VK_FORMAT_EAC_R11_SNORM_BLOCK:
        return "VK_FORMAT_EAC_R11_SNORM_BLOCK: A one-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of signed normalized red texel data.";
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
        return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK: A two-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized red and green texel data.";
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
        return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK: A two-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of signed normalized red and green texel data.";
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 5 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 5 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 5 x 5 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 5 x 5 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 6 x 5 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 6 x 5 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 6 x 6 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 6 x 6 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 5 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 5 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 6 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 6 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 8 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 8 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 5 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 5 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 6 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 6 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 8 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 8 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 10 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 10 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 12 x 10 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 12 x 10 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 12 x 12 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 12 x 12 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_G8B8G8R8_422_UNORM:
        return "VK_FORMAT_G8B8G8R8_422_UNORM: A four-component, packed format that has an 8-bit G component in the first 8 bits, an 8-bit B component in the next 8 bits, an 8-bit G component in the next 8 bits, and an 8-bit R component in the last "
               "8 bits.";
    case VK_FORMAT_B8G8R8G8_422_UNORM:
        return "VK_FORMAT_B8G8R8G8_422_UNORM: A four-component, packed format that has an 8-bit B component in the first 8 bits, an 8-bit G component in the next 8 bits, an 8-bit R component in the next 8 bits, and an 8-bit G component in the last "
               "8 bits.";
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: A three-component, planar format that has an 8-bit G component plane, an 8-bit B component plane, and an 8-bit R component plane.";
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: A three-component, planar format that has an 8-bit G component plane and an 8-bit B/R component plane.";
    case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM: A three-component, planar format that has an 8-bit G component plane, an 8-bit B component plane, and an 8-bit R component plane.";
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM: A three-component, planar format that has an 8-bit G component plane and an 8-bit B/R component plane.";
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM: A three-component, planar format that has an 8-bit G component plane, an 8-bit B component plane, and an 8-bit R component plane.";
    case VK_FORMAT_R10X6_UNORM_PACK16:
        return "VK_FORMAT_R10X6_UNORM_PACK16: A one-component, 16-bit unsigned normalized format that has a single 10-bit R component in the first 10 bits and a 6-bit X component in the last 6 bits.";
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
        return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16: A two-component, 32-bit unsigned normalized format that has a 10-bit R component in the first 10 bits, a 6-bit X component in the next 6 bits, a 10-bit G component in the next 10 bits, and a "
               "6-bit X component in the last 6 bits.";
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
        return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16: A four-component, 64-bit unsigned normalized format that has a 10-bit R component in the first 10 bits, a 6-bit X component in the next 6 bits, a 10-bit G component in the next 10 "
               "bits, a 6-bit X component in the next 6 bits, a 10-bit B component in the next 10 bits, a 6-bit X component in the next 6 bits, a 10-bit A component in the next 10 bits, and a 6-bit X component in the last 6 bits.";
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
        return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: A four-component, 64-bit unsigned normalized format that has a 10-bit G component in the first 10 bits, a 6-bit X component in the next 6 bits, a 10-bit B component in the next "
               "10 bits, a 6-bit X component in the next 6 bits, a 10-bit G component in the next 10 bits, a 6-bit X component in the next 6 bits, a 10-bit R component in the next 10 bits, and a 6-bit X component in the last 6 bits.";
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
        return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: A four-component, 64-bit unsigned normalized format that has a 10-bit B component in the first 10 bits, a 6-bit X component in the next 6 bits, a 10-bit G component in the next "
               "10 bits, a 6-bit X component in the next 6 bits, a 10-bit R component in the next 10 bits, a 6-bit X component in the next 6 bits, a 10-bit G component in the next 10 bits, and a 6-bit X component in the last 6 bits.";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 10-bit G component plane, a 6-bit X component plane, a 10-bit B component plane, a 6-bit X component plane, "
               "a 10-bit R component plane, and a 6-bit X component plane.";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 10-bit G component plane, a 6-bit X component plane, a 10-bit B/R component plane, and a 6-bit X component "
               "plane.";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 10-bit G component plane, a 6-bit X component plane, a 10-bit B component plane, a 6-bit X component "
               "plane, a 10-bit R component plane, and a 6-bit X component plane.";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 10-bit G component plane, a 6-bit X component plane, a 10-bit B/R component plane, and a 6-bit X "
               "component plane.";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 10-bit G component plane, a 6-bit X component plane, a 10-bit B component plane, a 6-bit X component "
               "plane, a 10-bit R component plane, and a 6-bit X component plane.";
    case VK_FORMAT_R12X4_UNORM_PACK16:
        return "VK_FORMAT_R12X4_UNORM_PACK16: A one-component, 16-bit unsigned normalized format that has a single 12-bit R component in the first 12 bits and a 4-bit X component in the last 4 bits.";
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
        return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16: A two-component, 32-bit unsigned normalized format that has a 12-bit R component in the first 12 bits, a 4-bit X component in the next 4 bits, a 12-bit G component in the next 12 bits, and a 4-bit "
               "X component in the last 4 bits.";
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
        return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16: A four-component, 64-bit unsigned normalized format that has a 12-bit R component in the first 12 bits, a 4-bit X component in the next 4 bits, a 12-bit G component in the next 12 bits, "
               "a 4-bit X component in the next 4 bits, a 12-bit B component in the next 12 bits, a 4-bit X component in the next 4 bits, a 12-bit A component in the next 12 bits, and a 4-bit X component in the last 4 bits.";
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
        return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: A four-component, 64-bit unsigned normalized format that has a 12-bit G component in the first 12 bits, a 4-bit X component in the next 4 bits, a 12-bit B component in the next 12 "
               "bits, a 4-bit X component in the next 4 bits, a 12-bit G component in the next 12 bits, a 4-bit X component in the next 4 bits, a 12-bit R component in the next 12 bits, and a 4-bit X component in the last 4 bits.";
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
        return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: A four-component, 64-bit unsigned normalized format that has a 12-bit B component in the first 12 bits, a 4-bit X component in the next 4 bits, a 12-bit G component in the next 12 "
               "bits, a 4-bit X component in the next 4 bits, a 12-bit R component in the next 12 bits, a 4-bit X component in the next 4 bits, a 12-bit G component in the next 12 bits, and a 4-bit X component in the last 4 bits.";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 12-bit G component plane, a 4-bit X component plane, a 12-bit B component plane, a 4-bit X component plane, a "
               "12-bit R component plane, and a 4-bit X component plane.";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 12-bit G component plane, a 4-bit X component plane, a 12-bit B/R component plane, and a 4-bit X component plane.";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 12-bit G component plane, a 4-bit X component plane, a 12-bit B component plane, a 4-bit X component plane, a "
               "12-bit R component plane, and a 4-bit X component plane.";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 12-bit G component plane, a 4-bit X component plane, a 12-bit B/R component plane, and a 4-bit X component plane.";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: A three-component, 48-bit unsigned normalized format that has a 12-bit G component plane, a 4-bit X component plane, a 12-bit B component plane, a 4-bit X component plane, a "
               "12-bit R component plane, and a 4-bit X component plane.";
    case VK_FORMAT_G16B16G16R16_422_UNORM:
        return "VK_FORMAT_G16B16G16R16_422_UNORM: A four-component, packed format that has a 16-bit G component in the first 16 bits, a 16-bit B component in the next 16 bits, a 16-bit G component in the next 16 bits, and a 16-bit R component in "
               "the last "
               "16 bits.";
    case VK_FORMAT_B16G16R16G16_422_UNORM:
        return "VK_FORMAT_B16G16R16G16_422_UNORM: A four-component, packed format that has a 16-bit B component in the first 16 bits, a 16-bit G component in the next 16 bits, a 16-bit R component in the next 16 bits, and a 16-bit G component in "
               "the last "
               "16 bits.";
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM: A three-component, planar format that has a 16-bit G component plane, a 16-bit B component plane, and a 16-bit R component plane.";
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM: A three-component, planar format that has a 16-bit G component plane and a 16-bit B/R component plane.";
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM: A three-component, planar format that has a 16-bit G component plane, a 16-bit B component plane, and a 16-bit R component plane.";
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM: A three-component, planar format that has a 16-bit G component plane and a 16-bit B/R component plane.";
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM: A three-component, planar format that has a 16-bit G component plane, a 16-bit B component plane, and a 16-bit R component plane.";
    case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of unsigned normalized RGBA texel data.";
    case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG: A four-component, block-compressed format where each 64-bit compressed texel block encodes a 4 x 4 rectangle of sRGB RGBA texel data.";
    case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 4 x 4 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 5 x 4 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 5 x 5 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 6 x 5 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 6 x 6 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 5 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 6 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 8 x 8 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 5 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 6 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 8 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 10 x 10 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 12 x 10 rectangle of 32-bit floating-point RGBA texel data.";
    case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT: A four-component, block-compressed format where each 128-bit compressed texel block encodes a 12 x 12 rectangle of 32-bit floating-point RGBA texel data.";
    default:
        return "UNKNOWN: An unknown error has occurred.";
    }
}

VkFormat VulkanTypeConverter::ToVkFormat(TextureFormat format)
{
    FN("VulkanTypeConverter::ToVkFormat");

    switch (format)
    {
    case TextureFormat::RedGreenBlue8:
        return VK_FORMAT_R8G8B8_UNORM;
    case TextureFormat::RedGreenBlue16:
        return VK_FORMAT_R16G16B16_UNORM;
    case TextureFormat::RedGreenBlueAlpha8:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::RedGreenBlueAlpha16:
        return VK_FORMAT_R16G16B16A16_UNORM;
    case TextureFormat::Depth16:
        return VK_FORMAT_D16_UNORM;
    case TextureFormat::Depth24:
        return VK_FORMAT_X8_D24_UNORM_PACK32;
    case TextureFormat::Red8:
        return VK_FORMAT_R8_UNORM;
    case TextureFormat::Red16:
        return VK_FORMAT_R16_UNORM;
    case TextureFormat::RedGreen8:
        return VK_FORMAT_R8G8_UNORM;
    case TextureFormat::RedGreen16:
        return VK_FORMAT_R16G16_UNORM;
    case TextureFormat::Red16f:
        return VK_FORMAT_R16_SFLOAT;
    case TextureFormat::Red32f:
        return VK_FORMAT_R32_SFLOAT;
    case TextureFormat::RedGreen16f:
        return VK_FORMAT_R16G16_SFLOAT;
    case TextureFormat::RedGreen32f:
        return VK_FORMAT_R32G32_SFLOAT;
    case TextureFormat::Red8i:
        return VK_FORMAT_R8_SINT;
    case TextureFormat::Red8ui:
        return VK_FORMAT_R8_UINT;
    case TextureFormat::Red16i:
        return VK_FORMAT_R16_SINT;
    case TextureFormat::Red16ui:
        return VK_FORMAT_R16_UINT;
    case TextureFormat::Red32i:
        return VK_FORMAT_R32_SINT;
    case TextureFormat::Red32ui:
        return VK_FORMAT_R32_UINT;
    case TextureFormat::RedGreen8i:
        return VK_FORMAT_R8G8_SINT;
    case TextureFormat::RedGreen8ui:
        return VK_FORMAT_R8G8_UINT;
    case TextureFormat::RedGreen16i:
        return VK_FORMAT_R16G16_SINT;
    case TextureFormat::RedGreen16ui:
        return VK_FORMAT_R16G16_UINT;
    case TextureFormat::RedGreen32i:
        return VK_FORMAT_R32G32_SINT;
    case TextureFormat::RedGreen32ui:
        return VK_FORMAT_R32G32_UINT;
    case TextureFormat::RedGreenBlueAlpha32f:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case TextureFormat::RedGreenBlue32f:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case TextureFormat::RedGreenBlueAlpha16f:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case TextureFormat::RedGreenBlue16f:
        return VK_FORMAT_R16G16B16_SFLOAT;
    case TextureFormat::Depth24Stencil8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case TextureFormat::Red11fGreen11fBlue10f:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case TextureFormat::RedGreenBlue9E5:
        return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    case TextureFormat::SRedGreenBlue8:
        return VK_FORMAT_R8G8B8_SNORM;
    case TextureFormat::SRedGreenBlue8Alpha8:
        return VK_FORMAT_R8G8B8A8_SNORM;
    case TextureFormat::Depth32f:
        return VK_FORMAT_D32_SFLOAT;
    case TextureFormat::Depth32fStencil8:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case TextureFormat::RedGreenBlueAlpha32ui:
        return VK_FORMAT_R32G32B32A32_UINT;
    case TextureFormat::RedGreenBlue32ui:
        return VK_FORMAT_R32G32B32_UINT;
    case TextureFormat::RedGreenBlueAlpha16ui:
        return VK_FORMAT_R16G16B16A16_UINT;
    case TextureFormat::RedGreenBlue16ui:
        return VK_FORMAT_R16G16B16_UINT;
    case TextureFormat::RedGreenBlueAlpha8ui:
        return VK_FORMAT_R8G8B8A8_UINT;
    case TextureFormat::RedGreenBlue8ui:
        return VK_FORMAT_R8G8B8_UINT;
    case TextureFormat::RedGreenBlueAlpha32i:
        return VK_FORMAT_R32G32B32A32_SINT;
    case TextureFormat::RedGreenBlue32i:
        return VK_FORMAT_R32G32B32_SINT;
    case TextureFormat::RedGreenBlueAlpha16i:
        return VK_FORMAT_R16G16B16A16_SINT;
    case TextureFormat::RedGreenBlue16i:
        return VK_FORMAT_R16G16B16_SINT;
    case TextureFormat::RedGreenBlueAlpha8i:
        return VK_FORMAT_R8G8B8A8_SINT;
    case TextureFormat::RedGreenBlue8i:
        return VK_FORMAT_R8G8B8_SINT;
    case TextureFormat::RedGreenBlue4Alpha4:
        return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    case TextureFormat::BlueGreenRed4Alpha4:
        return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
    case TextureFormat::Red5Green6Blue5:
        return VK_FORMAT_R5G6B5_UNORM_PACK16;
    case TextureFormat::Blue5Green6Red5:
        return VK_FORMAT_B5G6R5_UNORM_PACK16;
    case TextureFormat::Red5Green5Blue5Alpha1:
        return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
    case TextureFormat::Blue5Green5Red5Alpha1:
        return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    case TextureFormat::Alpha1Red5Green5Blue5:
        return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
    case TextureFormat::Alpha2Red10Green10Blue10:
        return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case TextureFormat::Alpha2Blue10Green10Red10:
        return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case TextureFormat::Blue8Green8Red8:
        return VK_FORMAT_B8G8R8_UNORM;
    case TextureFormat::Blue8Green8Red8Alpha8:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::Alpha8Blue8Green8Red8:
        return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    case TextureFormat::Blue10Green11Red11:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case TextureFormat::Blue9Green9Red9E5:
        return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    default:
        LOG_FATAL("TextureFormat does not exist.");
        return VK_FORMAT_UNDEFINED;
    }
}

TextureFormat VulkanTypeConverter::ToTextureFormat(VkFormat format)
{
    FN("VulkanTypeConverter::ToTextureFormat");

    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return TextureFormat::Undefined;
    case VK_FORMAT_R8G8B8_UNORM:
        return TextureFormat::RedGreenBlue8;
    case VK_FORMAT_R16G16B16_UNORM:
        return TextureFormat::RedGreenBlue16;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return TextureFormat::RedGreenBlueAlpha8;
    case VK_FORMAT_R16G16B16A16_UNORM:
        return TextureFormat::RedGreenBlueAlpha16;
    case VK_FORMAT_D16_UNORM:
        return TextureFormat::Depth16;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return TextureFormat::Depth24Stencil8;
    case VK_FORMAT_D32_SFLOAT:
        return TextureFormat::Depth32f;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return TextureFormat::Depth32fStencil8;
    case VK_FORMAT_R8_UNORM:
        return TextureFormat::Red8;
    case VK_FORMAT_R16_UNORM:
        return TextureFormat::Red16;
    case VK_FORMAT_R8G8_UNORM:
        return TextureFormat::RedGreen8;
    case VK_FORMAT_R16G16_UNORM:
        return TextureFormat::RedGreen16;
    case VK_FORMAT_R16_SFLOAT:
        return TextureFormat::Red16f;
    case VK_FORMAT_R32_SFLOAT:
        return TextureFormat::Red32f;
    case VK_FORMAT_R16G16_SFLOAT:
        return TextureFormat::RedGreen16f;
    case VK_FORMAT_R32G32_SFLOAT:
        return TextureFormat::RedGreen32f;
    case VK_FORMAT_R8_SINT:
        return TextureFormat::Red8i;
    case VK_FORMAT_R8_UINT:
        return TextureFormat::Red8ui;
    case VK_FORMAT_R16_SINT:
        return TextureFormat::Red16i;
    case VK_FORMAT_R16_UINT:
        return TextureFormat::Red16ui;
    case VK_FORMAT_R32_SINT:
        return TextureFormat::Red32i;
    case VK_FORMAT_R32_UINT:
        return TextureFormat::Red32ui;
    case VK_FORMAT_R8G8_SINT:
        return TextureFormat::RedGreen8i;
    case VK_FORMAT_R8G8_UINT:
        return TextureFormat::RedGreen8ui;
    case VK_FORMAT_R16G16_SINT:
        return TextureFormat::RedGreen16i;
    case VK_FORMAT_R16G16_UINT:
        return TextureFormat::RedGreen16ui;
    case VK_FORMAT_R32G32_SINT:
        return TextureFormat::RedGreen32i;
    case VK_FORMAT_R32G32_UINT:
        return TextureFormat::RedGreen32ui;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return TextureFormat::RedGreenBlueAlpha32f;
    case VK_FORMAT_R32G32B32_SFLOAT:
        return TextureFormat::RedGreenBlue32f;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return TextureFormat::RedGreenBlueAlpha16f;
    case VK_FORMAT_R16G16B16_SFLOAT:
        return TextureFormat::RedGreenBlue16f;
    case VK_FORMAT_R8G8B8_SNORM:
        return TextureFormat::SRedGreenBlue8;
    case VK_FORMAT_R8G8B8A8_SNORM:
        return TextureFormat::SRedGreenBlue8Alpha8;
    case VK_FORMAT_R32G32B32A32_UINT:
        return TextureFormat::RedGreenBlueAlpha32ui;
    case VK_FORMAT_R32G32B32_UINT:
        return TextureFormat::RedGreenBlue32ui;
    case VK_FORMAT_R16G16B16A16_UINT:
        return TextureFormat::RedGreenBlueAlpha16ui;
    case VK_FORMAT_R16G16B16_UINT:
        return TextureFormat::RedGreenBlue16ui;
    case VK_FORMAT_R8G8B8A8_UINT:
        return TextureFormat::RedGreenBlueAlpha8ui;
    case VK_FORMAT_R8G8B8_UINT:
        return TextureFormat::RedGreenBlue8ui;
    case VK_FORMAT_R32G32B32A32_SINT:
        return TextureFormat::RedGreenBlueAlpha32i;
    case VK_FORMAT_R32G32B32_SINT:
        return TextureFormat::RedGreenBlue32i;
    case VK_FORMAT_R16G16B16A16_SINT:
        return TextureFormat::RedGreenBlueAlpha16i;
    case VK_FORMAT_R16G16B16_SINT:
        return TextureFormat::RedGreenBlue16i;
    case VK_FORMAT_R8G8B8A8_SINT:
        return TextureFormat::RedGreenBlueAlpha8i;
    case VK_FORMAT_R8G8B8_SINT:
        return TextureFormat::RedGreenBlue8i;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        return TextureFormat::RedGreenBlue4Alpha4;
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
        return TextureFormat::BlueGreenRed4Alpha4;
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return TextureFormat::Red5Green6Blue5;
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
        return TextureFormat::Blue5Green6Red5;
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
        return TextureFormat::Red5Green5Blue5Alpha1;
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
        return TextureFormat::Blue5Green5Red5Alpha1;
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        return TextureFormat::Alpha1Red5Green5Blue5;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        return TextureFormat::Alpha2Red10Green10Blue10;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return TextureFormat::Alpha2Blue10Green10Red10;
    case VK_FORMAT_B8G8R8_UNORM:
        return TextureFormat::Blue8Green8Red8;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return TextureFormat::Blue8Green8Red8Alpha8;
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        return TextureFormat::Alpha8Blue8Green8Red8;
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        return TextureFormat::Blue10Green11Red11;
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
        return TextureFormat::Blue9Green9Red9E5;
    default:
        LOG_FATAL("TextureFormat does not exist.");
        return TextureFormat::Undefined;
    }
}

ImageFormat VulkanTypeConverter::TextureToImageFormat(TextureFormat format)
{
    FN("VulkanTypeConverter::TextureToImageFormat");

    switch (format)
    {
    case TextureFormat::RedGreenBlue8:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlue16:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha8:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlueAlpha16:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::Depth16:
        return ImageFormat::DepthComponent;
    case TextureFormat::Depth24:
        return ImageFormat::DepthComponent;
    case TextureFormat::Red8:
        return ImageFormat::Red;
    case TextureFormat::Red16:
        return ImageFormat::Red;
    case TextureFormat::RedGreen8:
        return ImageFormat::RedGreen;
    case TextureFormat::RedGreen16:
        return ImageFormat::RedGreen;
    case TextureFormat::Red16f:
        return ImageFormat::Red;
    case TextureFormat::Red32f:
        return ImageFormat::Red;
    case TextureFormat::RedGreen16f:
        return ImageFormat::RedGreen;
    case TextureFormat::RedGreen32f:
        return ImageFormat::RedGreen;
    case TextureFormat::Red8i:
        return ImageFormat::RedInteger;
    case TextureFormat::Red8ui:
        return ImageFormat::RedInteger;
    case TextureFormat::Red16i:
        return ImageFormat::RedInteger;
    case TextureFormat::Red16ui:
        return ImageFormat::RedInteger;
    case TextureFormat::Red32i:
        return ImageFormat::RedInteger;
    case TextureFormat::Red32ui:
        return ImageFormat::RedInteger;
    case TextureFormat::RedGreen8i:
        return ImageFormat::RedGreenInteger;
    case TextureFormat::RedGreen8ui:
        return ImageFormat::RedGreenInteger;
    case TextureFormat::RedGreen16i:
        return ImageFormat::RedGreenInteger;
    case TextureFormat::RedGreen16ui:
        return ImageFormat::RedGreenInteger;
    case TextureFormat::RedGreen32i:
        return ImageFormat::RedGreenInteger;
    case TextureFormat::RedGreen32ui:
        return ImageFormat::RedGreenInteger;
    case TextureFormat::RedGreenBlueAlpha32f:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue32f:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha16f:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue16f:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::Depth24Stencil8:
        return ImageFormat::DepthStencil;
    case TextureFormat::Red11fGreen11fBlue10f:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlue9E5:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::SRedGreenBlue8:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::SRedGreenBlue8Alpha8:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::Depth32f:
        return ImageFormat::DepthComponent;
    case TextureFormat::Depth32fStencil8:
        return ImageFormat::DepthStencil;
    case TextureFormat::RedGreenBlueAlpha32ui:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue32ui:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha16ui:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue16ui:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha8ui:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue8ui:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha32i:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue32i:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha16i:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue16i:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlueAlpha8i:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::RedGreenBlue8i:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::RedGreenBlue4Alpha4:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::BlueGreenRed4Alpha4:
        return ImageFormat::BlueGreenRedAlpha;
    case TextureFormat::Red5Green6Blue5:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::Blue5Green6Red5:
        return ImageFormat::BlueGreenRed;
    case TextureFormat::Red5Green5Blue5Alpha1:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::Blue5Green5Red5Alpha1:
        return ImageFormat::BlueGreenRedAlpha;
    case TextureFormat::Alpha1Red5Green5Blue5:
        return ImageFormat::RedGreenBlueAlpha;
    case TextureFormat::Alpha2Red10Green10Blue10:
        return ImageFormat::RedGreenBlue;
    case TextureFormat::Alpha2Blue10Green10Red10:
        return ImageFormat::BlueGreenRed;
    case TextureFormat::Blue8Green8Red8:
        return ImageFormat::BlueGreenRed;
    case TextureFormat::Blue8Green8Red8Alpha8:
        return ImageFormat::BlueGreenRedAlpha;
    case TextureFormat::Alpha8Blue8Green8Red8:
        return ImageFormat::BlueGreenRedAlpha;
    case TextureFormat::Blue10Green11Red11:
        return ImageFormat::BlueGreenRed;
    case TextureFormat::Blue9Green9Red9E5:
        return ImageFormat::BlueGreenRed;
    default:
        LOG_FATAL("TextureFormat does not exist.");
        return ImageFormat::RedGreenBlue;
    }
}

VkFormat VulkanTypeConverter::ToVkFormat(ShaderVertexAttributeType type)
{
    FN("VulkanTypeConverter::ToVkFormat");

    switch (type)
    {
    case ShaderVertexAttributeType::Float:
        return VK_FORMAT_R32_SFLOAT;
    case ShaderVertexAttributeType::FloatVector2:
        return VK_FORMAT_R32G32_SFLOAT;
    case ShaderVertexAttributeType::FloatVector3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderVertexAttributeType::FloatVector4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ShaderVertexAttributeType::FloatMatrix22:
        return VK_FORMAT_R32G32_SFLOAT;
    case ShaderVertexAttributeType::FloatMatrix33:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderVertexAttributeType::FloatMatrix44:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ShaderVertexAttributeType::Int:
        return VK_FORMAT_R32_SINT;
    case ShaderVertexAttributeType::IntVector2:
        return VK_FORMAT_R32G32_SINT;
    case ShaderVertexAttributeType::IntVector3:
        return VK_FORMAT_R32G32B32_SINT;
    case ShaderVertexAttributeType::IntVector4:
        return VK_FORMAT_R32G32B32A32_SINT;
    default:
        LOG_FATAL("ActiveAttribType does not exist.");
        return VK_FORMAT_UNDEFINED;
    }
}

VkFilter VulkanTypeConverter::ToVkFilter(TextureMagnificationFilter filter)
{
    FN("VulkanTypeConverter::ToVkFilter");

    switch (filter)
    {
    case TextureMagnificationFilter::Nearest:
        return VK_FILTER_NEAREST;
    case TextureMagnificationFilter::Linear:
        return VK_FILTER_LINEAR;
    default:
        LOG_FATAL("TextureMagnificationFilter does not exist.");
        return VK_FILTER_NEAREST;
    }
}

VkFilter VulkanTypeConverter::ToVkFilter(TextureMinificationFilter filter)
{
    FN("VulkanTypeConverter::ToVkFilter");

    switch (filter)
    {
    case TextureMinificationFilter::Nearest:
        return VK_FILTER_NEAREST;
    case TextureMinificationFilter::Linear:
        return VK_FILTER_LINEAR;
    case TextureMinificationFilter::NearestMipmapNearest:
        return VK_FILTER_NEAREST;
    case TextureMinificationFilter::LinearMipmapNearest:
        return VK_FILTER_LINEAR;
    case TextureMinificationFilter::NearestMipmapLinear:
        return VK_FILTER_NEAREST;
    case TextureMinificationFilter::LinearMipmapLinear:
        return VK_FILTER_LINEAR;
    default:
        LOG_FATAL("TextureMinificationFilter does not exist.");
        return VK_FILTER_NEAREST;
    }
}

VkSamplerAddressMode VulkanTypeConverter::ToVkSamplerAddressMode(TextureWrap wrap)
{
    FN("VulkanTypeConverter::ToVkSamplerAddressMode");

    switch (wrap)
    {
    case TextureWrap::Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case TextureWrap::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case TextureWrap::MirroredRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    default:
        LOG_FATAL("TextureWrap does not exist.");
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    }
}

VkBufferUsageFlags VulkanTypeConverter::ToVkBufferUsageFlags(BufferHint hint)
{
    FN("VulkanTypeConverter::ToVkBufferUsageFlags");

    switch (hint)
    {
    case BufferHint::StreamDraw:
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferHint::StreamRead:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    case BufferHint::StreamCopy:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferHint::StaticDraw:
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferHint::StaticRead:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    case BufferHint::StaticCopy:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferHint::DynamicDraw:
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferHint::DynamicRead:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    case BufferHint::DynamicCopy:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    case BufferHint::None:
    default:
        return 0;
    }
}

VkIndexType VulkanTypeConverter::ToVkIndexType(IndexBufferDatatype dataType)
{
    FN("VulkanTypeConverter::ToVkIndexType");

    switch (dataType)
    {
    case IndexBufferDatatype::UnsignedInt8:
        return VK_INDEX_TYPE_UINT8_EXT;
    case IndexBufferDatatype::UnsignedInt16:
        return VK_INDEX_TYPE_UINT16;
    case IndexBufferDatatype::UnsignedInt32:
        return VK_INDEX_TYPE_UINT32;
    default:
        LOG_FATAL("IndexBufferDatatype does not exist.");
        return VK_INDEX_TYPE_NONE_KHR;
    }
}

ShaderVertexAttributeType VulkanTypeConverter::ToShaderVertexAttributeType(const spirv_cross::SPIRType &type)
{
    FN("VulkanTypeConverter::ToShaderVertexAttributeType");

    if (type.basetype == spirv_cross::SPIRType::Float)
    {
        if (type.columns == 1)
        {
            switch (type.vecsize)
            {
            case 1:
                return ShaderVertexAttributeType::Float;
            case 2:
                return ShaderVertexAttributeType::FloatVector2;
            case 3:
                return ShaderVertexAttributeType::FloatVector3;
            case 4:
                return ShaderVertexAttributeType::FloatVector4;
            }
        }
        else if (type.columns == 2 && type.vecsize == 2)
        {
            return ShaderVertexAttributeType::FloatMatrix22;
        }
        else if (type.columns == 3 && type.vecsize == 3)
        {
            return ShaderVertexAttributeType::FloatMatrix33;
        }
        else if (type.columns == 4 && type.vecsize == 4)
        {
            return ShaderVertexAttributeType::FloatMatrix44;
        }
    }
    else if (type.basetype == spirv_cross::SPIRType::Int)
    {
        switch (type.vecsize)
        {
        case 1:
            return ShaderVertexAttributeType::Int;
        case 2:
            return ShaderVertexAttributeType::IntVector2;
        case 3:
            return ShaderVertexAttributeType::IntVector3;
        case 4:
            return ShaderVertexAttributeType::IntVector4;
        }
    }

    LOG_FATAL("ActiveAttribType does not exist.");
    return ShaderVertexAttributeType::INVALID;
}

VkPrimitiveTopology VulkanTypeConverter::ToVkPrimitiveTopology(PrimitiveType primitiveType)
{
    FN("VulkanTypeConverter::ToVkPrimitiveTopology");

    switch (primitiveType)
    {
    case PrimitiveType::Points:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case PrimitiveType::Lines:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveType::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case PrimitiveType::Triangles:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveType::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case PrimitiveType::TriangleFan:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    default:
        LOG_FATAL("PrimitiveType does not exist.");
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkBlendFactor VulkanTypeConverter::ToVkBlendFactor(SourceBlendingFactor factor)
{
    FN("VulkanTypeConverter::ToVkBlendFactor");

    switch (factor)
    {
    case SourceBlendingFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case SourceBlendingFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case SourceBlendingFactor::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case SourceBlendingFactor::OneMinusSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case SourceBlendingFactor::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case SourceBlendingFactor::OneMinusDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case SourceBlendingFactor::DestinationColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case SourceBlendingFactor::OneMinusDestinationColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case SourceBlendingFactor::SourceAlphaSaturate:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case SourceBlendingFactor::ConstantColor:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case SourceBlendingFactor::OneMinusConstantColor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case SourceBlendingFactor::ConstantAlpha:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case SourceBlendingFactor::OneMinusConstantAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    default:
        LOG_FATAL("SourceBlendingFactor does not exist.");
        return VK_BLEND_FACTOR_ONE;
    }
}

VkBlendFactor VulkanTypeConverter::ToVkBlendFactor(DestinationBlendingFactor factor)
{
    FN("VulkanTypeConverter::ToVkBlendFactor");

    switch (factor)
    {
    case DestinationBlendingFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case DestinationBlendingFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case DestinationBlendingFactor::SourceColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case DestinationBlendingFactor::OneMinusSourceColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case DestinationBlendingFactor::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case DestinationBlendingFactor::OneMinusSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case DestinationBlendingFactor::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case DestinationBlendingFactor::OneMinusDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case DestinationBlendingFactor::DestinationColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case DestinationBlendingFactor::OneMinusDestinationColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case DestinationBlendingFactor::ConstantColor:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case DestinationBlendingFactor::OneMinusConstantColor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case DestinationBlendingFactor::ConstantAlpha:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case DestinationBlendingFactor::OneMinusConstantAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    default:
        LOG_FATAL("DestinationBlendingFactor does not exist.");
        return VK_BLEND_FACTOR_ONE;
    }
}

VkBlendOp VulkanTypeConverter::ToVkBlendOp(BlendEquation equation)
{
    FN("VulkanTypeConverter::ToVkBlendOp");

    switch (equation)
    {
    case BlendEquation::Add:
        return VK_BLEND_OP_ADD;
    case BlendEquation::Minimum:
        return VK_BLEND_OP_MIN;
    case BlendEquation::Maximum:
        return VK_BLEND_OP_MAX;
    case BlendEquation::Subtract:
        return VK_BLEND_OP_SUBTRACT;
    case BlendEquation::ReverseSubtract:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    default:
        LOG_FATAL("BlendEquation does not exist.");
        return VK_BLEND_OP_ADD;
    }
}

VkCompareOp VulkanTypeConverter::ToVkCompareOp(DepthTestFunction function)
{
    FN("VulkanTypeConverter::ToVkCompareOp");

    switch (function)
    {
    case DepthTestFunction::Never:
        return VK_COMPARE_OP_NEVER;
    case DepthTestFunction::Less:
        return VK_COMPARE_OP_LESS;
    case DepthTestFunction::Equal:
        return VK_COMPARE_OP_EQUAL;
    case DepthTestFunction::LessThanOrEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case DepthTestFunction::Greater:
        return VK_COMPARE_OP_GREATER;
    case DepthTestFunction::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case DepthTestFunction::GreaterThanOrEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case DepthTestFunction::Always:
        return VK_COMPARE_OP_ALWAYS;
    default:
        LOG_FATAL("DepthTestFunction does not exist.");
        return VK_COMPARE_OP_NEVER;
    }
}

VkCompareOp VulkanTypeConverter::ToVkCompareOp(StencilTestFunction function)
{
    FN("VulkanTypeConverter::ToVkCompareOp");

    switch (function)
    {
    case StencilTestFunction::Never:
        return VK_COMPARE_OP_NEVER;
    case StencilTestFunction::Less:
        return VK_COMPARE_OP_LESS;
    case StencilTestFunction::Equal:
        return VK_COMPARE_OP_EQUAL;
    case StencilTestFunction::LessThanOrEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case StencilTestFunction::Greater:
        return VK_COMPARE_OP_GREATER;
    case StencilTestFunction::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case StencilTestFunction::GreaterThanOrEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case StencilTestFunction::Always:
        return VK_COMPARE_OP_ALWAYS;
    default:
        LOG_FATAL("StencilTestFunction does not exist.");
        return VK_COMPARE_OP_NEVER;
    }
}

VkStencilOp VulkanTypeConverter::ToVkStencilOp(StencilOperation operation)
{
    FN("VulkanTypeConverter::ToVkStencilOp");

    switch (operation)
    {
    case StencilOperation::Zero:
        return VK_STENCIL_OP_ZERO;
    case StencilOperation::Invert:
        return VK_STENCIL_OP_INVERT;
    case StencilOperation::Keep:
        return VK_STENCIL_OP_KEEP;
    case StencilOperation::Replace:
        return VK_STENCIL_OP_REPLACE;
    case StencilOperation::Increment:
        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case StencilOperation::Decrement:
        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case StencilOperation::IncrementWrap:
        return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOperation::DecrementWrap:
        return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    default:
        LOG_FATAL("StencilOperation does not exist.");
        return VK_STENCIL_OP_KEEP;
    }
}

VkCullModeFlags VulkanTypeConverter::ToVkCullMode(CullFace cullFace)
{
    FN("VulkanTypeConverter::ToVkCullMode");

    switch (cullFace)
    {
    case CullFace::Front:
        return VK_CULL_MODE_FRONT_BIT;
    case CullFace::Back:
        return VK_CULL_MODE_BACK_BIT;
    case CullFace::FrontAndBack:
        return VK_CULL_MODE_FRONT_AND_BACK;
    default:
        LOG_FATAL("CullFace does not exist.");
        return VK_CULL_MODE_BACK_BIT;
    }
}

VkFrontFace VulkanTypeConverter::ToVkFrontFace(WindingOrder windingOrder)
{
    FN("VulkanTypeConverter::ToVkFrontFace");

    switch (windingOrder)
    {
    case WindingOrder::Clockwise:
        return VK_FRONT_FACE_CLOCKWISE;
    case WindingOrder::Counterclockwise:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    default:
        LOG_FATAL("WindingOrder does not exist.");
        return VK_FRONT_FACE_CLOCKWISE;
    }
}

VkPolygonMode VulkanTypeConverter::ToVkPolygonMode(RasterizationMode mode)
{
    FN("VulkanTypeConverter::ToVkPolygonMode");

    switch (mode)
    {
    case RasterizationMode::Point:
        return VK_POLYGON_MODE_POINT;
    case RasterizationMode::Line:
        return VK_POLYGON_MODE_LINE;
    case RasterizationMode::Fill:
        return VK_POLYGON_MODE_FILL;
    default:
        LOG_FATAL("RasterizationMode does not exist.");
        return VK_POLYGON_MODE_FILL;
    }
}

VkBool32 VulkanTypeConverter::ToVkBool32(bool boolean)
{
    FN("VulkanTypeConverter::ToVkBool32");

    return boolean ? VK_TRUE : VK_FALSE;
}

} // namespace bow
