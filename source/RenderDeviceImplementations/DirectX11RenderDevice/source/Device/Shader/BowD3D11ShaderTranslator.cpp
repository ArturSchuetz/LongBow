#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderTranslator.h>

#include <CoreSystems/BowLogger.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

namespace bow
{
namespace
{

shaderc_shader_kind ToShaderKind(ShaderTranslator::Stage stage)
{
    switch (stage)
    {
    case ShaderTranslator::Stage::Vertex:
        return shaderc_vertex_shader;
    case ShaderTranslator::Stage::Fragment:
        return shaderc_fragment_shader;
    case ShaderTranslator::Stage::Geometry:
        return shaderc_geometry_shader;
    case ShaderTranslator::Stage::Compute:
        return shaderc_compute_shader;
    }
    return shaderc_vertex_shader;
}

const char *StageName(ShaderTranslator::Stage stage)
{
    switch (stage)
    {
    case ShaderTranslator::Stage::Vertex:
        return "vertex";
    case ShaderTranslator::Stage::Fragment:
        return "fragment";
    case ShaderTranslator::Stage::Geometry:
        return "geometry";
    case ShaderTranslator::Stage::Compute:
        return "compute";
    }
    return "unknown";
}

} // namespace

bool ShaderTranslator::LooksLikeGLSL(const std::string &source) { return source.find("#version") != std::string::npos; }

ShaderTranslator::Result ShaderTranslator::ToHLSL(const std::string &source, Stage stage)
{
    FN("ShaderTranslator::ToHLSL");

    Result result;
    result.ok = false;

    if (!LooksLikeGLSL(source))
    {
        result.ok = true;
        result.hlsl = source;
        return result;
    }

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    // Vulkan semantics, because that is the dialect the examples are written
    // in -- layout(set = ...) and layout(push_constant) only mean something
    // here. SPIRV-Cross flattens both when it emits HLSL.
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
    options.SetOptimizationLevel(shaderc_optimization_level_zero);

    const shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, ToShaderKind(stage), StageName(stage), EntryPoint(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        result.message = module.GetErrorMessage();
        return result;
    }

    const std::vector<uint32_t> spirv(module.cbegin(), module.cend());

    try
    {
        spirv_cross::CompilerHLSL hlslCompiler(spirv);

        spirv_cross::CompilerHLSL::Options hlslOptions;
        // Shader Model 5.0 is what d3dcompiler accepts for DirectX 11.
        hlslOptions.shader_model = 50;
        // Without this a uniform block arrives as loose constants rather than
        // a cbuffer, which is not what the constant-buffer path expects.
        hlslOptions.flatten_matrix_vertex_input_semantics = false;
        hlslCompiler.set_hlsl_options(hlslOptions);

        spirv_cross::CompilerGLSL::Options commonOptions;
        // GLSL clip space is -1..1 in depth and its framebuffer origin is at
        // the bottom; DirectX uses 0..1 and the top. Letting SPIRV-Cross fix
        // both up front means the example's vertex shader does not have to
        // know which backend it ended up on.
        commonOptions.vertex.fixup_clipspace = true;
        commonOptions.vertex.flip_vert_y = true;
        hlslCompiler.set_common_options(commonOptions);

        // Read the stage input names before compiling: the HLSL that comes
        // out has TEXCOORD semantics and no trace of what they were called.
        const spirv_cross::ShaderResources resources = hlslCompiler.get_shader_resources();
        for (const spirv_cross::Resource &input : resources.stage_inputs)
        {
            const uint32_t location = hlslCompiler.get_decoration(input.id, spv::DecorationLocation);
            const std::string name = hlslCompiler.get_name(input.id);
            if (!name.empty())
            {
                result.attributeNames[location] = name;
            }
        }

        for (const spirv_cross::Resource &block : resources.storage_buffers)
        {
            const uint32_t binding = hlslCompiler.get_decoration(block.id, spv::DecorationBinding);
            // The instance name is what a caller binds by; the block type name
            // is the fallback when the block was declared without one.
            std::string name = hlslCompiler.get_name(block.id);
            if (name.empty())
            {
                name = hlslCompiler.get_name(block.base_type_id);
            }
            if (!name.empty())
            {
                result.storageBlockNames[binding] = name;
            }
        }

        result.hlsl = hlslCompiler.compile();
        result.ok = true;
    }
    catch (const std::exception &error)
    {
        result.message = std::string("SPIRV-Cross: ") + error.what();
    }

    return result;
}

} // namespace bow
