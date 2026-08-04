#
# Generated version header
#
# Exposed as <LongBow/LongBow-version.h> through the LongBow::Version interface
# target, so any module can report the version it was built from.
#

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/LongBow/LongBow-version.h"
    @ONLY
)

add_library(Version INTERFACE)
add_library(LongBow::Version ALIAS Version)
target_include_directories(Version INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/include")

add_subdirectory(ThirdParty)

#
# Engine modules
#
# CoreSystems -> Platform -> Resources are the foundation; the three device
# modules on top of them hold nothing but interfaces and a manager that opens
# the matching backend at runtime.
#

add_subdirectory(CoreSystems)
add_subdirectory(Platform)
add_subdirectory(Resources)
add_subdirectory(RenderDevice)
add_subdirectory(InputDevice)

#
# Backends
#

add_subdirectory(RenderDeviceImplementations)
add_subdirectory(InputDeviceImplementations)

if(LONGBOW_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()
