# OpenGL 3.x render backend. Windowing via GLFW, extension loading via GLEW.
# This is the reference implementation of IRenderDevice.

find_package(OpenGL REQUIRED)

longbow_add_plugin(OpenGL3xRenderDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::Resources
        LongBow::RenderDevice
        LongBow::Profiler
        GLEW::GLEW
        glfw
        OpenGL::GL
    PRIVATE_DEFS
        GLEW_STATIC
    FOLDER "Backends/Render"
)
