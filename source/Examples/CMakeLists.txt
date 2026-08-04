
# Check if examples are enabled
if(NOT OPTION_BUILD_EXAMPLES)
    return()
endif()

# Example applications
add_subdirectory(00_TimerTest)
add_subdirectory(01_Input)
add_subdirectory(02_HelloWorld)
add_subdirectory(03_Triangle)
add_subdirectory(04_Cube)
add_subdirectory(05_Textures)
add_subdirectory(06_MeshRendering)
#add_subdirectory(07_Framebuffer)
add_subdirectory(08_ComputeShader)
add_subdirectory(09_PathTracing)