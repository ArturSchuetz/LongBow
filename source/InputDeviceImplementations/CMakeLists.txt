# Input backends, opened by name at runtime by InputDeviceManager.
#
# NativeInputDevice goes through GLFW and works wherever GLFW does;
# DirectInputDevice talks to DirectInput and is Windows-only.

add_subdirectory(NativeInputDevice)

if(WIN32)
    add_subdirectory(DirectInputDevice)
endif()
