# Render backends. Each one builds to a shared library that RenderDeviceManager
# opens by name at runtime, so they are never linked into the engine itself.

if(LONGBOW_BUILD_OPENGL)
    add_subdirectory(OpenGL3xRenderDevice)
endif()

if(LONGBOW_BUILD_DIRECTX11)
    add_subdirectory(DirectX11RenderDevice)
endif()

if(LONGBOW_BUILD_DIRECTX12)
    add_subdirectory(DirectX12RenderDevice)
endif()

if(LONGBOW_BUILD_VULKAN)
    add_subdirectory(VulkanRenderDevice)
endif()
