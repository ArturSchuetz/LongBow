longbow_add_executable(Thin_01_HelloTriangle
    DEPS
        LongBow::CoreSystems
        LongBow::ThinRenderDevice
        LongBow::ExampleSupport
        glfw
    FOLDER "Examples/Thin"
)

longbow_add_hlsl_shader(Thin_01_HelloTriangle
    SOURCE Triangle.hlsl
    STAGE vertex
    ENTRY mainVS
    SPIRV DXIL)

longbow_add_hlsl_shader(Thin_01_HelloTriangle
    SOURCE Triangle.hlsl
    STAGE fragment
    ENTRY mainPS
    SPIRV DXIL)

if(TARGET VulkanThinDevice)
    add_dependencies(Thin_01_HelloTriangle VulkanThinDevice)
endif()
