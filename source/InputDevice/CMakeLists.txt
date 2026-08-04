# Keyboard and mouse abstractions plus the manager that opens the matching
# input backend at runtime. RenderDevice is needed for the window handle an
# input device attaches to.

longbow_add_library(InputDevice
    PUBLIC_DEPS
        LongBow::CoreSystems
        LongBow::RenderDevice
    FOLDER "Engine"
)
