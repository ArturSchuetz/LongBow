# Explicit rendering interfaces for Vulkan and DirectX 12.
#
# Deliberately separate from RenderDevice: those two APIs record command lists,
# own their synchronisation and build pipeline state up front, and squeezing
# that through an immediate-mode interface cost both tiers. See
# docs/ARCHITECTURE.md.

longbow_add_library(ThinRenderDevice
    PUBLIC_DEPS
        LongBow::CoreSystems
    FOLDER "Engine"
)
