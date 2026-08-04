# Vulkan render backend, including hardware ray tracing.
#
# GLSL is compiled to SPIR-V at runtime with shaderc and reflected with
# SPIRV-Cross. Both ship with the Vulkan SDK, so neither is vendored:
# shaderc_combined is a FindVulkan component, SPIRV-Cross is picked up from
# the SDK's lib directory below.

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

# spirv_cross_import(<target> <basename>)
#
# The SDK ships release and debug variants side by side, distinguished only by
# a trailing "d". Wiring both into one imported target keeps the debug build
# from mixing runtimes.
function(spirv_cross_import target basename)
    if(TARGET ${target})
        return()
    endif()

    find_library(${basename}_RELEASE NAMES ${basename}  HINTS "${Vulkan_INCLUDE_DIR}/../Lib" REQUIRED)
    find_library(${basename}_DEBUG   NAMES ${basename}d HINTS "${Vulkan_INCLUDE_DIR}/../Lib")
    mark_as_advanced(${basename}_RELEASE ${basename}_DEBUG)

    add_library(${target} STATIC IMPORTED)
    set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION "${${basename}_RELEASE}"
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}"
    )
    if(${basename}_DEBUG)
        set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_DEBUG "${${basename}_DEBUG}")
    endif()
endfunction()

spirv_cross_import(SPIRVCross::core spirv-cross-core)
spirv_cross_import(SPIRVCross::glsl spirv-cross-glsl)
spirv_cross_import(SPIRVCross::msl  spirv-cross-msl)

longbow_add_plugin(VulkanRenderDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::Resources
        LongBow::RenderDevice
        LongBow::Profiler
        Vulkan::Vulkan
        Vulkan::shaderc_combined
        SPIRVCross::core
        SPIRVCross::glsl
        SPIRVCross::msl
        glfw
    FOLDER "Backends/Render"
)
