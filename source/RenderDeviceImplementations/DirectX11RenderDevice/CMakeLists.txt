# DirectX 11 render backend. Windows only.
#
# shaderc and SPIRV-Cross come in because the examples are authored in GLSL and
# this backend translates them: GLSL -> SPIR-V -> HLSL -> DXBC. Both ship with
# the Vulkan SDK, so nothing new is vendored.

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

function(d3d11_spirv_cross_import target basename)
    if(TARGET ${target})
        return()
    endif()
    find_library(D3D11_${basename}_RELEASE NAMES ${basename}  HINTS "${Vulkan_INCLUDE_DIR}/../Lib" REQUIRED)
    find_library(D3D11_${basename}_DEBUG   NAMES ${basename}d HINTS "${Vulkan_INCLUDE_DIR}/../Lib")
    mark_as_advanced(D3D11_${basename}_RELEASE D3D11_${basename}_DEBUG)
    add_library(${target} STATIC IMPORTED)
    set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION "${D3D11_${basename}_RELEASE}"
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}")
    if(D3D11_${basename}_DEBUG)
        set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_DEBUG "${D3D11_${basename}_DEBUG}")
    endif()
endfunction()

d3d11_spirv_cross_import(D3D11SPIRVCross::core spirv-cross-core)
d3d11_spirv_cross_import(D3D11SPIRVCross::glsl spirv-cross-glsl)
d3d11_spirv_cross_import(D3D11SPIRVCross::hlsl spirv-cross-hlsl)
#
# Unlike the OpenGL and Vulkan backends this creates its own Win32 window
# rather than going through GLFW, because DXGI wants a plain HWND.

longbow_add_plugin(DirectX11RenderDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::Resources
        LongBow::RenderDevice
        LongBow::Profiler
        d3d11
        dxgi
        d3dcompiler
        Vulkan::shaderc_combined
        D3D11SPIRVCross::hlsl
        D3D11SPIRVCross::glsl
        D3D11SPIRVCross::core
    FOLDER "Backends/Render"
)
