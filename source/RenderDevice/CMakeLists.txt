# Rendering abstractions: IRenderDevice, IRenderContext, buffers, textures,
# shaders, render states and acceleration structures -- plus the manager that
# opens the matching backend plugin at runtime. Contains no graphics API code.

longbow_add_library(RenderDevice
    PUBLIC_DEPS
        LongBow::CoreSystems
        LongBow::Resources
    FOLDER "Engine"
)
