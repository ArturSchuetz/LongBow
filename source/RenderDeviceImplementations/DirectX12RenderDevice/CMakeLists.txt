# DirectX 12 render backend. Windows only.
# GLFW provides the window; rendering goes through a raw swap chain on its
# native HWND.

longbow_add_plugin(DirectX12RenderDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::Resources
        LongBow::RenderDevice
        LongBow::Profiler
        glfw
        d3d12
        dxgi
        d3dcompiler
    FOLDER "Backends/Render"
)
