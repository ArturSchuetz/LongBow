# Helpers for declaring LongBow modules.
#
# Every module lives in source/<Name>/ and follows the same layout:
#
#     source/<Name>/include/<Name>/*.h    public headers, included as <Name/Foo.h>
#     source/<Name>/source/*.cpp          implementation, may have private headers
#
# so the two functions below can derive everything from the module name.

include(GenerateExportHeader)

# longbow_add_library(<name>
#     [STATIC|SHARED]
#     [PUBLIC_DEPS   <target>...]   propagated to consumers
#     [PRIVATE_DEPS  <target>...]   implementation detail
#     [PUBLIC_DEFS   <define>...]
#     [PRIVATE_DEFS  <define>...]
#     [FOLDER        <ide-folder>]
# )
#
# Creates the target <name> plus the alias LongBow::<name>, and generates
# include/<name>/<name>_api.h defining the <NAME>_API visibility macro.
function(longbow_add_library name)
    cmake_parse_arguments(ARG
        "STATIC;SHARED"
        "FOLDER"
        "PUBLIC_DEPS;PRIVATE_DEPS;PUBLIC_DEFS;PRIVATE_DEFS"
        ${ARGN})

    if(ARG_STATIC)
        set(kind STATIC)
    elseif(ARG_SHARED)
        set(kind SHARED)
    else()
        set(kind "")  # follow BUILD_SHARED_LIBS
    endif()

    set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include/${name}")
    set(source_dir  "${CMAKE_CURRENT_SOURCE_DIR}/source")

    file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${include_dir}/*.h" "${include_dir}/*.inl")
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${source_dir}/*.cpp" "${source_dir}/*.c" "${source_dir}/*.h")

    if(NOT sources)
        message(FATAL_ERROR "longbow_add_library(${name}): no sources found under ${source_dir}")
    endif()

    add_library(${name} ${kind} ${sources} ${headers})
    add_library(LongBow::${name} ALIAS ${name})

    # Public headers are addressed as <Name/Foo.h>, so the include root is one
    # level above include/<Name>. The generated _api.h lands in the matching
    # spot inside the build tree, hence the second interface directory.
    target_include_directories(${name}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${source_dir}
    )

    target_link_libraries(${name}
        PUBLIC  ${ARG_PUBLIC_DEPS}
        PRIVATE ${ARG_PRIVATE_DEPS}
    )

    target_compile_definitions(${name}
        PUBLIC  ${ARG_PUBLIC_DEFS}
        PRIVATE ${ARG_PRIVATE_DEFS}
    )

    string(TOUPPER ${name} name_upper)
    generate_export_header(${name}
        BASE_NAME        ${name_upper}
        EXPORT_MACRO_NAME ${name_upper}_API
        EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/${name}/${name}_api.h"
    )

    set_target_properties(${name} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        DEBUG_POSTFIX "${LONGBOW_DEBUG_POSTFIX}"
        FOLDER "${ARG_FOLDER}"
    )

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources} ${headers})
endfunction()

# longbow_add_plugin(<name> ...)
#
# Same as longbow_add_library, but always SHARED: render, input and network
# backends are opened at runtime with LoadLibrary/dlopen and resolved through a
# single exported factory function, so they must never be static.
function(longbow_add_plugin name)
    longbow_add_library(${name} SHARED ${ARGN})
endfunction()

# longbow_add_executable(<name> [DEPS <target>...] [FOLDER <ide-folder>])
#
# Used for examples and tools. Sources are globbed from the calling directory.
function(longbow_add_executable name)
    cmake_parse_arguments(ARG "" "FOLDER" "DEPS" ${ARGN})

    file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

    if(NOT sources)
        message(FATAL_ERROR "longbow_add_executable(${name}): no sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    add_executable(${name} ${sources})
    target_link_libraries(${name} PRIVATE ${ARG_DEPS})
    set_target_properties(${name} PROPERTIES
        FOLDER "${ARG_FOLDER}"
        VS_DEBUGGER_WORKING_DIRECTORY "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}"
    )
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})
endfunction()
