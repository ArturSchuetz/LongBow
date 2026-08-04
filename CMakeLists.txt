cmake_minimum_required(VERSION 3.21)

project(LongBow
    VERSION     2.0.0
    DESCRIPTION "Modular C++17 rendering framework with pluggable OpenGL, DirectX and Vulkan backends"
    HOMEPAGE_URL "https://github.com/ArturSchuetz/LongBow"
    LANGUAGES   C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(LongBowModule)

# Only meaningful when LongBow is the top-level project; as a subdirectory the
# parent decides about output layout, IDE folders and testing.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LONGBOW_IS_TOP_LEVEL ON)
else()
    set(LONGBOW_IS_TOP_LEVEL OFF)
endif()

#
# Platform capability detection
#
# The backend options default to what this machine can actually build, so a
# plain `cmake -B build` succeeds everywhere and simply produces fewer
# backends on a machine without, say, the Vulkan SDK.
#

find_package(Vulkan QUIET)

set(LONGBOW_CAN_BUILD_VULKAN    ${Vulkan_FOUND})
set(LONGBOW_CAN_BUILD_DIRECTX11 ${WIN32})
set(LONGBOW_CAN_BUILD_DIRECTX12 ${WIN32})
set(LONGBOW_CAN_BUILD_OPENGL    ON)

option(LONGBOW_BUILD_OPENGL    "Build the OpenGL 3.x render backend"  ${LONGBOW_CAN_BUILD_OPENGL})
option(LONGBOW_BUILD_DIRECTX11 "Build the DirectX 11 render backend"  ${LONGBOW_CAN_BUILD_DIRECTX11})
option(LONGBOW_BUILD_DIRECTX12 "Build the DirectX 12 render backend"  ${LONGBOW_CAN_BUILD_DIRECTX12})
option(LONGBOW_BUILD_VULKAN    "Build the Vulkan render backend"      ${LONGBOW_CAN_BUILD_VULKAN})
option(LONGBOW_BUILD_EXAMPLES  "Build the example applications"       ${LONGBOW_IS_TOP_LEVEL})
option(LONGBOW_BUILD_TESTS     "Build the test suite"                 OFF)
option(LONGBOW_BUILD_DOCS      "Build the Doxygen documentation"      OFF)
option(LONGBOW_ENABLE_PROFILER "Enable Optick profiler instrumentation" OFF)

foreach(backend VULKAN DIRECTX11 DIRECTX12)
    if(LONGBOW_BUILD_${backend} AND NOT LONGBOW_CAN_BUILD_${backend})
        message(FATAL_ERROR
            "LONGBOW_BUILD_${backend}=ON but this platform cannot build it. "
            "DirectX needs Windows; Vulkan needs the Vulkan SDK (set VULKAN_SDK).")
    endif()
endforeach()

#
# Output layout
#
# Executables and the backend plugins must share one directory: the managers
# locate a backend by bare filename via LoadLibrary/dlopen, so a plugin that
# lands anywhere else is simply not found at runtime.
#

set(LONGBOW_DEBUG_POSTFIX "d")

if(LONGBOW_IS_TOP_LEVEL)
    set(LONGBOW_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

    # Multi-config generators append the configuration name by default, which
    # would split executables and plugins into sibling directories.
    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${config} config_upper)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} "${PROJECT_BINARY_DIR}/lib/${config}")
    endforeach()

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
endif()

if(MSVC)
    add_compile_options(/MP /permissive- /utf-8)
    # 4251: STL types in the interface of exported classes. The engine ships
    # its plugins and consumers from one build, so the ABI always matches.
    add_compile_options(/wd4251)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
endif()

if(LONGBOW_IS_TOP_LEVEL AND LONGBOW_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(source)

if(LONGBOW_BUILD_DOCS)
    add_subdirectory(docs)
endif()

#
# Configuration summary
#

message(STATUS "")
message(STATUS "LongBow ${PROJECT_VERSION}")
message(STATUS "  Render backends:")
message(STATUS "    OpenGL 3.x   ${LONGBOW_BUILD_OPENGL}")
message(STATUS "    DirectX 11   ${LONGBOW_BUILD_DIRECTX11}")
message(STATUS "    DirectX 12   ${LONGBOW_BUILD_DIRECTX12}")
message(STATUS "    Vulkan       ${LONGBOW_BUILD_VULKAN}")
message(STATUS "  Examples       ${LONGBOW_BUILD_EXAMPLES}")
message(STATUS "  Tests          ${LONGBOW_BUILD_TESTS}")
message(STATUS "  Profiler       ${LONGBOW_ENABLE_PROFILER}")
message(STATUS "")
