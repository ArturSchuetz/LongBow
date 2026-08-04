# Third-party dependencies.
#
# Only LoadPNG is vendored -- it is a single translation unit and has no build
# system worth fetching. Everything else is either pulled in at configure time
# or taken from the Vulkan SDK, so the repository stays free of binaries and
# the dependencies match the compiler actually in use.

include(FetchContent)

set(CMAKE_FOLDER "ThirdParty")

#
# LoadPNG -- PNG decoding for Resources
#

add_library(LoadPNG STATIC LoadPNG/source/lodepng.cpp LoadPNG/include/LoadPNG/lodepng.h)
add_library(LongBow::LoadPNG ALIAS LoadPNG)
target_include_directories(LoadPNG PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/LoadPNG/include")
# The vendored header carries a generate_export_header block that defaults to
# __declspec(dllimport); this tells it we are linking the static variant.
target_compile_definitions(LoadPNG PUBLIC LOADPNG_STATIC_DEFINE)
set_target_properties(LoadPNG PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    FOLDER "ThirdParty"
)
# lodepng is upstream code; its warnings are not actionable here.
if(MSVC)
    target_compile_options(LoadPNG PRIVATE /W0)
else()
    target_compile_options(LoadPNG PRIVATE -w)
endif()

#
# Optick -- frame profiler
#
# Consumers always link LongBow::Profiler and always `#include <optick.h>`.
# Which of the two implementations that resolves to is decided here.
#

add_library(Profiler INTERFACE)
add_library(LongBow::Profiler ALIAS Profiler)

if(LONGBOW_ENABLE_PROFILER)
    FetchContent_Declare(optick
        GIT_REPOSITORY https://github.com/bombomby/optick.git
        GIT_TAG        1.4.0
        GIT_SHALLOW    ON
    )
    set(OPTICK_INSTALL_TARGETS OFF CACHE BOOL "" FORCE)
    set(OPTICK_BUILD_GUI_APP   OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(optick)
    target_link_libraries(Profiler INTERFACE OptickCore)
else()
    target_include_directories(Profiler INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/OptickStub/include")
endif()

#
# GLFW -- windowing for the OpenGL and Vulkan backends, and the window handle
# both input backends attach to. Always needed.
#

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    ON
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

#
# GLEW -- OpenGL extension loading
#
# Upstream nigels-com/glew needs a code-generation step before it can be built
# from a checkout; Perlmint's mirror ships the generated sources plus CMake.
#

if(LONGBOW_BUILD_OPENGL)
    # An installed GLEW (system package, vcpkg, Homebrew) is preferred; the
    # fetch below is the fallback for machines without one.
    #
    # GLOBAL matters: find_package creates IMPORTED targets scoped to the
    # calling directory, and the backend that consumes GLEW lives in a sibling
    # directory, where an unscoped GLEW::GLEW would not be visible.
    find_package(GLEW QUIET GLOBAL)

    if(NOT TARGET GLEW::GLEW)
        FetchContent_Declare(glew
            GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
            GIT_TAG        glew-cmake-2.2.0
            GIT_SHALLOW    ON
        )
        set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(ONLY_LIBS               ON  CACHE BOOL "" FORCE)

        # glew-cmake still declares cmake_minimum_required(VERSION 2.8), which
        # CMake 4 refuses outright. The floor only applies while this
        # subproject configures.
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        FetchContent_MakeAvailable(glew)
        unset(CMAKE_POLICY_VERSION_MINIMUM)

        # The mirror names its static target libglew_static; give it the
        # spelling the rest of the tree uses.
        if(TARGET libglew_static)
            add_library(GLEW::GLEW ALIAS libglew_static)
        endif()
    endif()
endif()
