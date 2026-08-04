# Example applications.
#
# Each one picks its render backend at runtime through RenderDeviceManager, so
# none of them links a backend directly -- the plugins just have to be built
# and sit next to the executable.

add_subdirectory(ExampleSupport)

add_subdirectory(00_TimerTest)
add_subdirectory(01_Input)
add_subdirectory(02_HelloWorld)
add_subdirectory(03_Triangle)
add_subdirectory(04_Cube)
add_subdirectory(05_Textures)
add_subdirectory(06_MeshRendering)
add_subdirectory(07_Framebuffer)
add_subdirectory(08_ComputeShader)
add_subdirectory(09_PathTracing)

# Thin-tier examples
add_subdirectory(Thin)

# The examples are useless without a backend to load, so make sure whichever
# ones are enabled are built before them.
foreach(example
    00_TimerTest 01_Input 02_HelloWorld 03_Triangle 04_Cube
    05_Textures 06_MeshRendering 07_Framebuffer 08_ComputeShader 09_PathTracing)
    foreach(backend
        OpenGLRenderDevice DirectX11RenderDevice DirectX12RenderDevice
        VulkanRenderDevice NativeInputDevice DirectInputDevice)
        if(TARGET ${backend})
            add_dependencies(${example} ${backend})
        endif()
    endforeach()
endforeach()
