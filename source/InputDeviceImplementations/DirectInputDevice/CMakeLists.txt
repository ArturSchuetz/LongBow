# Input backend built on DirectInput. Windows only.
# GLFW is still needed to obtain the native window handle to attach to.

longbow_add_plugin(DirectInputDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::InputDevice
        LongBow::RenderDevice
        LongBow::Profiler
        glfw
        dinput8
        dxguid
    FOLDER "Backends/Input"
)
