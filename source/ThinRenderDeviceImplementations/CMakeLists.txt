# Thin backends. Each builds to a shared library that ThinRenderDeviceManager
# opens by name at runtime and resolves CreateThinDevice out of.

if(LONGBOW_BUILD_VULKAN)
    add_subdirectory(VulkanThinDevice)
endif()

# DirectX 12 is written against this interface in stage 6; until the directory
# exists there is nothing to add.
if(LONGBOW_BUILD_DIRECTX12 AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/DirectX12ThinDevice")
    add_subdirectory(DirectX12ThinDevice)
endif()
