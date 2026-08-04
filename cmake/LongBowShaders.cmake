# Compiling shaders at build time.
#
# The thin tier is authored in HLSL because dxc emits both DXIL for DirectX 12
# and SPIR-V for Vulkan from one source, which removes the need for two shader
# sets. The classic tier keeps GLSL for OpenGL, which the driver compiles, and
# the DirectX 11 backend translates that at load time.
#
# Everything is compiled here rather than at runtime so that a shipped example
# needs no compiler present.

find_program(LONGBOW_DXC_EXECUTABLE
    NAMES dxc
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin"
    DOC "DirectX Shader Compiler, used for HLSL to DXIL and HLSL to SPIR-V")

find_program(LONGBOW_GLSLC_EXECUTABLE
    NAMES glslc
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin"
    DOC "glslc, used for GLSL to SPIR-V")

mark_as_advanced(LONGBOW_DXC_EXECUTABLE LONGBOW_GLSLC_EXECUTABLE)

# longbow_shader_profile(<out> <stage> <model>)
#
# dxc names a profile <stage>_<model>, e.g. vs_6_0. The stage abbreviations are
# not the same as the file suffixes, hence the mapping.
function(longbow_shader_profile out stage model)
    if(stage STREQUAL "vertex")
        set(prefix "vs")
    elseif(stage STREQUAL "fragment" OR stage STREQUAL "pixel")
        set(prefix "ps")
    elseif(stage STREQUAL "geometry")
        set(prefix "gs")
    elseif(stage STREQUAL "compute")
        set(prefix "cs")
    elseif(stage STREQUAL "raygeneration" OR stage STREQUAL "closesthit" OR stage STREQUAL "miss" OR stage STREQUAL "anyhit" OR stage STREQUAL "intersection")
        # Ray tracing shaders all compile as a library and are separated by
        # their entry point rather than by profile.
        set(prefix "lib")
    else()
        message(FATAL_ERROR "longbow_shader_profile: unknown stage '${stage}'")
    endif()

    set(${out} "${prefix}_${model}" PARENT_SCOPE)
endfunction()

# longbow_add_hlsl_shader(<target>
#     SOURCE <file> STAGE <stage> [ENTRY <name>] [MODEL <6_0>]
#     [SPIRV] [DXIL]
# )
#
# Compiles one HLSL file into whichever outputs are asked for, next to the
# executable under shaders/. Both forms come from the same source, which is the
# point of authoring the thin tier in HLSL.
function(longbow_add_hlsl_shader target)
    cmake_parse_arguments(ARG "SPIRV;DXIL" "SOURCE;STAGE;ENTRY;MODEL" "" ${ARGN})

    if(NOT ARG_SOURCE OR NOT ARG_STAGE)
        message(FATAL_ERROR "longbow_add_hlsl_shader: SOURCE and STAGE are required")
    endif()

    if(NOT LONGBOW_DXC_EXECUTABLE)
        message(WARNING "dxc was not found, so ${ARG_SOURCE} cannot be compiled. Install the Vulkan SDK or set LONGBOW_DXC_EXECUTABLE.")
        return()
    endif()

    if(NOT ARG_ENTRY)
        set(ARG_ENTRY "main")
    endif()
    if(NOT ARG_MODEL)
        set(ARG_MODEL "6_0")
    endif()

    longbow_shader_profile(profile "${ARG_STAGE}" "${ARG_MODEL}")

    get_filename_component(name "${ARG_SOURCE}" NAME_WE)
    set(source "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SOURCE}")
    set(outputDirectory "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}/shaders")

    set(outputs)

    if(ARG_SPIRV)
        set(spirv "${outputDirectory}/${name}.${ARG_STAGE}.spv")
        add_custom_command(
            OUTPUT "${spirv}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${outputDirectory}"
            COMMAND "${LONGBOW_DXC_EXECUTABLE}" -spirv -T ${profile} -E ${ARG_ENTRY} -fvk-use-dx-layout -Fo "${spirv}" "${source}"
            DEPENDS "${source}"
            COMMENT "HLSL to SPIR-V: ${ARG_SOURCE}"
            VERBATIM)
        list(APPEND outputs "${spirv}")
    endif()

    if(ARG_DXIL)
        set(dxil "${outputDirectory}/${name}.${ARG_STAGE}.dxil")
        add_custom_command(
            OUTPUT "${dxil}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${outputDirectory}"
            COMMAND "${LONGBOW_DXC_EXECUTABLE}" -T ${profile} -E ${ARG_ENTRY} -Fo "${dxil}" "${source}"
            DEPENDS "${source}"
            COMMENT "HLSL to DXIL: ${ARG_SOURCE}"
            VERBATIM)
        list(APPEND outputs "${dxil}")
    endif()

    if(outputs)
        target_sources(${target} PRIVATE ${outputs} "${source}")
        set_source_files_properties(${outputs} PROPERTIES GENERATED TRUE)
        # The source itself must not be handed to the C++ compiler.
        set_source_files_properties("${source}" PROPERTIES HEADER_FILE_ONLY TRUE)
    endif()
endfunction()

# longbow_add_glsl_shader(<target> SOURCE <file> STAGE <stage>)
#
# GLSL to SPIR-V, for OpenGL builds that want to consume SPIR-V through
# GL_ARB_gl_spirv rather than hand the driver source.
function(longbow_add_glsl_shader target)
    cmake_parse_arguments(ARG "" "SOURCE;STAGE" "" ${ARGN})

    if(NOT LONGBOW_GLSLC_EXECUTABLE)
        message(WARNING "glslc was not found, so ${ARG_SOURCE} cannot be compiled.")
        return()
    endif()

    get_filename_component(name "${ARG_SOURCE}" NAME_WE)
    set(source "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SOURCE}")
    set(outputDirectory "${LONGBOW_RUNTIME_OUTPUT_DIRECTORY}/shaders")
    set(spirv "${outputDirectory}/${name}.${ARG_STAGE}.glsl.spv")

    add_custom_command(
        OUTPUT "${spirv}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${outputDirectory}"
        COMMAND "${LONGBOW_GLSLC_EXECUTABLE}" -fshader-stage=${ARG_STAGE} -o "${spirv}" "${source}"
        DEPENDS "${source}"
        COMMENT "GLSL to SPIR-V: ${ARG_SOURCE}"
        VERBATIM)

    target_sources(${target} PRIVATE "${spirv}" "${source}")
    set_source_files_properties("${spirv}" PROPERTIES GENERATED TRUE)
    set_source_files_properties("${source}" PROPERTIES HEADER_FILE_ONLY TRUE)
endfunction()
