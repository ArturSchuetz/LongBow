# DirectX 11 render backend. Windows only.
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
    FOLDER "Backends/Render"
)
