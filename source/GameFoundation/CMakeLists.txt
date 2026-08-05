# Actors and components on top of the renderer and resource modules.
#
# Present in this repository's history but never in any build; it is in one now.

longbow_add_library(GameFoundation
    PUBLIC_DEPS
        LongBow::CoreSystems
        LongBow::Resources
        LongBow::RenderDevice
    FOLDER "Engine"
)
