#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderProgram.h>

#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderResourceBindings.h>
#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderTranslator.h>

#include <CoreSystems/BowLogger.h>

#include <d3dcompiler.h>

namespace bow
{
namespace
{

//! Maps a reflected HLSL input type onto the engine's attribute types.
ShaderVertexAttributeType ToAttributeType(BYTE componentType, BYTE componentCount)
{
    if (componentType == D3D_REGISTER_COMPONENT_FLOAT32)
    {
        switch (componentCount)
        {
        case 1:
            return ShaderVertexAttributeType::Float;
        case 2:
            return ShaderVertexAttributeType::FloatVector2;
        case 3:
            return ShaderVertexAttributeType::FloatVector3;
        default:
            return ShaderVertexAttributeType::FloatVector4;
        }
    }
    return ShaderVertexAttributeType::Int;
}

//! Number of components in a reflected input's component mask.
BYTE ComponentCount(BYTE mask)
{
    BYTE count = 0;
    while (mask)
    {
        count += (mask & 1);
        mask >>= 1;
    }
    return count;
}

} // namespace

D3D11ShaderProgram::D3D11ShaderProgram(ID3D11Device *device, ID3D11DeviceContext *context, const std::string &vertexSource, const std::string &fragmentSource, const std::string &geometrySource)
    : m_device(device), m_context(context), m_ready(false)
{
    FN("D3D11ShaderProgram::D3D11ShaderProgram");

    const ShaderTranslator::Result vertex = ShaderTranslator::ToHLSL(vertexSource, ShaderTranslator::Stage::Vertex);
    if (!vertex.ok)
    {
        LOG_ERROR("Could not translate the vertex shader: %s", vertex.message.c_str());
        return;
    }

    const ShaderTranslator::Result fragment = ShaderTranslator::ToHLSL(fragmentSource, ShaderTranslator::Stage::Fragment);
    if (!fragment.ok)
    {
        LOG_ERROR("Could not translate the fragment shader: %s", fragment.message.c_str());
        return;
    }

    Microsoft::WRL::ComPtr<ID3DBlob> pixelByteCode;
    if (!CompileStage(vertex.hlsl, "vs_5_0", m_vertexByteCode) || !CompileStage(fragment.hlsl, "ps_5_0", pixelByteCode))
    {
        return;
    }

    HRESULT result = device->CreateVertexShader(m_vertexByteCode->GetBufferPointer(), m_vertexByteCode->GetBufferSize(), nullptr, &m_vertexShader);
    if (FAILED(result))
    {
        LOG_ERROR("CreateVertexShader failed (0x%08X).", (unsigned)result);
        return;
    }

    result = device->CreatePixelShader(pixelByteCode->GetBufferPointer(), pixelByteCode->GetBufferSize(), nullptr, &m_pixelShader);
    if (FAILED(result))
    {
        LOG_ERROR("CreatePixelShader failed (0x%08X).", (unsigned)result);
        return;
    }

    if (!geometrySource.empty())
    {
        const ShaderTranslator::Result geometry = ShaderTranslator::ToHLSL(geometrySource, ShaderTranslator::Stage::Geometry);
        Microsoft::WRL::ComPtr<ID3DBlob> geometryByteCode;
        if (geometry.ok && CompileStage(geometry.hlsl, "gs_5_0", geometryByteCode))
        {
            device->CreateGeometryShader(geometryByteCode->GetBufferPointer(), geometryByteCode->GetBufferSize(), nullptr, &m_geometryShader);
        }
        else
        {
            LOG_ERROR("Could not build the geometry shader: %s", geometry.message.c_str());
        }
    }

    ReflectVertexInput(m_vertexByteCode.Get(), vertex.attributeNames);
    ReflectResources(m_vertexByteCode.Get(), true);
    ReflectResources(pixelByteCode.Get(), false);

    m_ready = true;
    LOG_TRACE("Shader successfully linked!");
}

D3D11ShaderProgram::~D3D11ShaderProgram() { FN("D3D11ShaderProgram::~D3D11ShaderProgram"); }

bool D3D11ShaderProgram::CompileStage(const std::string &source, const char *target, Microsoft::WRL::ComPtr<ID3DBlob> &byteCode)
{
    FN("D3D11ShaderProgram::CompileStage");

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    const HRESULT result = D3DCompile(source.c_str(), source.size(), nullptr, nullptr, nullptr, ShaderTranslator::EntryPoint(), target, flags, 0, &byteCode, &errors);

    if (FAILED(result))
    {
        if (errors != nullptr)
        {
            LOG_ERROR("%s: %s", target, static_cast<const char *>(errors->GetBufferPointer()));
        }
        else
        {
            LOG_ERROR("D3DCompile failed for %s (0x%08X).", target, (unsigned)result);
        }
        return false;
    }

    return true;
}

void D3D11ShaderProgram::ReflectVertexInput(ID3DBlob *byteCode, const std::unordered_map<uint32_t, std::string> &originalNames)
{
    FN("D3D11ShaderProgram::ReflectVertexInput");

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
    if (FAILED(D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_PPV_ARGS(&reflection))))
    {
        LOG_ERROR("Could not reflect the vertex shader.");
        return;
    }

    D3D11_SHADER_DESC shaderDescription = {};
    reflection->GetDesc(&shaderDescription);

    LOG_TRACE("\tVertexAttributes:");

    for (UINT i = 0; i < shaderDescription.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC parameter = {};
        reflection->GetInputParameterDesc(i, &parameter);

        // System-value inputs such as SV_VertexID are generated rather than
        // fed from a buffer, so they are not attributes.
        if (parameter.SystemValueType != D3D_NAME_UNDEFINED)
        {
            continue;
        }

        const BYTE components = ComponentCount(parameter.Mask);

        // SPIRV-Cross names translated inputs TEXCOORD<n> and carries the
        // original GLSL location in the semantic index, which is what the
        // engine addresses an attribute by.
        const int location = (int)parameter.SemanticIndex;

        // Prefer the name the shader was written with; fall back to the
        // semantic when the source was HLSL to begin with and there is none.
        std::string name;
        std::unordered_map<uint32_t, std::string>::const_iterator original = originalNames.find((uint32_t)location);
        if (original != originalNames.end())
        {
            name = original->second;
        }
        else
        {
            name = parameter.SemanticName;
            if (parameter.SemanticIndex != 0)
            {
                name += std::to_string(parameter.SemanticIndex);
            }
        }

        LOG_TRACE("\t\tName: %s, \tLocation: %d", name.c_str(), location);
        m_vertexAttributes.insert(std::make_pair(location, ShaderVertexAttributePtr(new ShaderVertexAttribute(location, name, ToAttributeType(parameter.ComponentType, components), components))));
    }
}

void D3D11ShaderProgram::ReflectResources(ID3DBlob *byteCode, bool vertexStage)
{
    FN("D3D11ShaderProgram::ReflectResources");

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
    if (FAILED(D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_PPV_ARGS(&reflection))))
    {
        return;
    }

    D3D11_SHADER_DESC shaderDescription = {};
    reflection->GetDesc(&shaderDescription);

    for (UINT i = 0; i < shaderDescription.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC binding = {};
        reflection->GetResourceBindingDesc(i, &binding);

        std::unordered_map<std::string, ResourceSlot> *target = nullptr;
        switch (binding.Type)
        {
        case D3D_SIT_CBUFFER:
            target = &m_constantBuffers;
            break;
        case D3D_SIT_TEXTURE:
            target = &m_textures;
            break;
        case D3D_SIT_SAMPLER:
            target = &m_samplers;
            break;
        default:
            continue;
        }

        // A resource used by both stages keeps one entry that says so, since
        // DirectX 11 binds per stage and the draw path has to set both.
        std::unordered_map<std::string, ResourceSlot>::iterator existing = target->find(binding.Name);
        if (existing != target->end())
        {
            existing->second.inVertexStage = existing->second.inVertexStage || vertexStage;
            existing->second.inPixelStage = existing->second.inPixelStage || !vertexStage;
            continue;
        }

        ResourceSlot slot;
        slot.slot = binding.BindPoint;
        slot.inVertexStage = vertexStage;
        slot.inPixelStage = !vertexStage;
        target->insert(std::make_pair(std::string(binding.Name), slot));
    }
}

ShaderVertexAttributePtr D3D11ShaderProgram::VGetVertexAttribute(std::string name)
{
    FN("D3D11ShaderProgram::VGetVertexAttribute");

    for (auto it = m_vertexAttributes.begin(); it != m_vertexAttributes.end(); ++it)
    {
        if (it->second->Name == name)
        {
            return it->second;
        }
    }

    return nullptr;
}

ShaderVertexAttributeMap D3D11ShaderProgram::VGetVertexAttributes()
{
    FN("D3D11ShaderProgram::VGetVertexAttributes");

    return m_vertexAttributes;
}

int D3D11ShaderProgram::VGetFragmentOutputLocation(std::string /*name*/)
{
    FN("D3D11ShaderProgram::VGetFragmentOutputLocation");

    // DirectX 11 addresses render targets by slot rather than by name, and
    // the only pixel shader output the examples use is the first one.
    return 0;
}

ShaderResourceBindingsPtr D3D11ShaderProgram::VCreateResourceBindingObjects()
{
    FN("D3D11ShaderProgram::VCreateResourceBindingObjects");

    return D3D11ShaderResourceBindingsPtr(new D3D11ShaderResourceBindings());
}

void D3D11ShaderProgram::VSetPushConstants(const char *name, const void *data, size_t offset, size_t size)
{
    FN("D3D11ShaderProgram::VSetPushConstants");

    if (data == nullptr || size == 0)
    {
        LOG_ERROR("No data given for push constant '%s'.", name);
        return;
    }

    // A Vulkan push-constant block arrives here as an ordinary constant
    // buffer, so the write goes into one allocated on first use.
    std::unordered_map<std::string, PushConstantBlock>::iterator block = m_pushConstantBlocks.find(name);
    if (block == m_pushConstantBlocks.end())
    {
        const ResourceSlot *reflected = FindConstantBuffer(name);
        if (reflected == nullptr)
        {
            LOG_ERROR("Shader has no constant buffer named '%s'.", name);
            return;
        }

        PushConstantBlock created;
        created.slot = reflected->slot;
        created.inVertexStage = reflected->inVertexStage;
        created.inPixelStage = reflected->inPixelStage;
        created.sizeInBytes = (offset + size + 15) & ~static_cast<size_t>(15);

        D3D11_BUFFER_DESC description = {};
        description.ByteWidth = (UINT)created.sizeInBytes;
        description.Usage = D3D11_USAGE_DYNAMIC;
        description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (FAILED(m_device->CreateBuffer(&description, nullptr, &created.buffer)))
        {
            LOG_ERROR("Could not create the constant buffer behind push constant '%s'.", name);
            return;
        }

        block = m_pushConstantBlocks.insert(std::make_pair(std::string(name), created)).first;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(m_context->Map(block->second.buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        LOG_ERROR("Could not map the constant buffer behind push constant '%s'.", name);
        return;
    }

    memcpy(static_cast<char *>(mapped.pData) + offset, data, size);
    m_context->Unmap(block->second.buffer.Get(), 0);

    ID3D11Buffer *buffers[] = {block->second.buffer.Get()};
    if (block->second.inVertexStage)
    {
        m_context->VSSetConstantBuffers(block->second.slot, 1, buffers);
    }
    if (block->second.inPixelStage)
    {
        m_context->PSSetConstantBuffers(block->second.slot, 1, buffers);
    }
}

void D3D11ShaderProgram::VSetPushConstants(ShaderStage /*shaderStage*/, const void *data, size_t offset, size_t size)
{
    FN("D3D11ShaderProgram::VSetPushConstants");

    // As on OpenGL, a push constant can only be addressed by name here. Route
    // the call when the shader has exactly one constant buffer, and refuse to
    // guess otherwise.
    if (m_constantBuffers.size() != 1)
    {
        LOG_ERROR("Cannot address a push constant by shader stage: the shader has %u constant buffers. Use the overload taking a name.", (unsigned)m_constantBuffers.size());
        return;
    }

    VSetPushConstants(m_constantBuffers.begin()->first.c_str(), data, offset, size);
}

void D3D11ShaderProgram::Bind()
{
    FN("D3D11ShaderProgram::Bind");

    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
}

const D3D11ShaderProgram::ResourceSlot *D3D11ShaderProgram::FindConstantBuffer(const std::string &name) const
{
    std::unordered_map<std::string, ResourceSlot>::const_iterator it = m_constantBuffers.find(name);
    return (it != m_constantBuffers.end()) ? &it->second : nullptr;
}

const D3D11ShaderProgram::ResourceSlot *D3D11ShaderProgram::FindTexture(const std::string &name) const
{
    std::unordered_map<std::string, ResourceSlot>::const_iterator it = m_textures.find(name);
    return (it != m_textures.end()) ? &it->second : nullptr;
}

const D3D11ShaderProgram::ResourceSlot *D3D11ShaderProgram::FindSampler(const std::string &name) const
{
    std::unordered_map<std::string, ResourceSlot>::const_iterator it = m_samplers.find(name);
    return (it != m_samplers.end()) ? &it->second : nullptr;
}

} // namespace bow
