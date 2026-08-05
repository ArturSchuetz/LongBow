# DirectX 12 behind the thin interface. Windows only.
#
# The counterpart of VulkanThinDevice against the same interface; where the two
# APIs disagree the difference is confined here.

longbow_add_plugin(DirectX12ThinDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::ThinRenderDevice
        d3d12
        dxgi
        d3dcompiler
    FOLDER "Backends/Thin"
)
