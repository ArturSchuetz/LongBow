#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <string>
#include <unordered_map>

namespace bow
{

//! Turns the shader source an example provides into something HLSL can compile.
/*!
    The examples are authored once for every backend and their shaders are
    GLSL. Rather than make every example carry a second, hand-written HLSL
    copy, this runs the translation the tooling already supports:

        GLSL --shaderc--> SPIR-V --SPIRV-Cross--> HLSL

    Both libraries ship with the Vulkan SDK and the Vulkan backend already
    links them, so this costs no new dependency. HLSL that is handed in
    directly is passed through untouched, which is how an example can provide
    its own when it wants to.
*/
class ShaderTranslator
{
  public:
    enum class Stage
    {
        Vertex,
        Fragment,
        Geometry,
        Compute
    };

    struct Result
    {
        bool ok;
        std::string hlsl;
        std::string message;

        //! Location to the attribute name the GLSL used.
        /*!
            SPIRV-Cross renames stage inputs to TEXCOORD<location> when it
            emits HLSL, because HLSL addresses them by semantic. Callers still
            look an attribute up by the name they wrote in the shader, so the
            original names are read out of the SPIR-V before that happens.
        */
        std::unordered_map<uint32_t, std::string> attributeNames;
    };

    //! Translates GLSL to Shader Model 5.0 HLSL, or passes HLSL through.
    /*!
        \param source Shader source, GLSL or HLSL.
        \param stage  Which pipeline stage the source is for.
        \return The HLSL, or ok == false and a message saying what failed.
    */
    static Result ToHLSL(const std::string &source, Stage stage);

    //! Whether the source looks like GLSL rather than HLSL.
    /*!
        A `#version` directive is the reliable marker: it is mandatory in GLSL
        for anything past the fixed pipeline and is not valid HLSL.
    */
    static bool LooksLikeGLSL(const std::string &source);

    //! Entry point name the translated HLSL uses.
    static const char *EntryPoint() { return "main"; }
};

} // namespace bow
