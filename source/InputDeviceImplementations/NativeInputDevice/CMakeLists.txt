# Input backend built on GLFW's keyboard and mouse callbacks.
# Portable wherever GLFW runs.

longbow_add_plugin(NativeInputDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::InputDevice
        LongBow::RenderDevice
        LongBow::Profiler
        glfw
    FOLDER "Backends/Input"
)
