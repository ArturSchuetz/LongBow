#include <DirectX11RenderDevice/Device/Shader/BowD3D11ComputeShaderProgram.h>

#include <DirectX11RenderDevice/Device/Buffer/BowD3D11StorageBuffer.h>
#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderResourceBindings.h>
#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderTranslator.h>

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <d3dcompiler.h>

namespace bow
{

D3D11ComputeShaderProgram::D3D11ComputeShaderProgram(ID3D11Device *device, ID3D11DeviceContext *context, const std::string &computeShaderSource) : m_device(device), m_context(context), m_ready(false)
{
    FN("D3D11ComputeShaderProgram::D3D11ComputeShaderProgram");

    const ShaderTranslator::Result compute = ShaderTranslator::ToHLSL(computeShaderSource, ShaderTranslator::Stage::Compute);
    if (!compute.ok)
    {
        LOG_ERROR("Could not translate the compute shader: %s", compute.message.c_str());
        return;
    }

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> byteCode;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    HRESULT result = D3DCompile(compute.hlsl.c_str(), compute.hlsl.size(), nullptr, nullptr, nullptr, ShaderTranslator::EntryPoint(), "cs_5_0", flags, 0, &byteCode, &errors);
    if (FAILED(result))
    {
        if (errors != nullptr)
        {
            LOG_ERROR("cs_5_0: %s", static_cast<const char *>(errors->GetBufferPointer()));
        }
        else
        {
            LOG_ERROR("D3DCompile failed for the compute shader (0x%08X).", (unsigned)result);
        }
        return;
    }

    result = device->CreateComputeShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_computeShader);
    if (FAILED(result))
    {
        LOG_ERROR("CreateComputeShader failed (0x%08X).", (unsigned)result);
        return;
    }

    Reflect(byteCode.Get(), compute.storageBlockNames);

    m_ready = true;
    LOG_TRACE("Compute shader successfully linked!");
}

D3D11ComputeShaderProgram::~D3D11ComputeShaderProgram() { FN("D3D11ComputeShaderProgram::~D3D11ComputeShaderProgram"); }

void D3D11ComputeShaderProgram::Reflect(ID3DBlob *byteCode, const std::unordered_map<uint32_t, std::string> &originalNames)
{
    FN("D3D11ComputeShaderProgram::Reflect");

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
    if (FAILED(D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_PPV_ARGS(&reflection))))
    {
        LOG_ERROR("Could not reflect the compute shader.");
        return;
    }

    D3D11_SHADER_DESC shaderDescription = {};
    reflection->GetDesc(&shaderDescription);

    for (UINT i = 0; i < shaderDescription.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC binding = {};
        reflection->GetResourceBindingDesc(i, &binding);

        // Register the block under the name the GLSL used, since that is what
        // a caller binds by. The HLSL name after translation is unrelated.
        std::string name = binding.Name;
        std::unordered_map<uint32_t, std::string>::const_iterator original = originalNames.find(binding.BindPoint);
        if (original != originalNames.end())
        {
            name = original->second;
        }

        switch (binding.Type)
        {
        case D3D_SIT_STRUCTURED:
            // Declared readonly in the shader, so a shader resource view.
            m_storageBlocks[name] = StorageSlot{binding.BindPoint, false};
            break;
        case D3D_SIT_UAV_RWSTRUCTURED:
        case D3D_SIT_UAV_RWTYPED:
        case D3D_SIT_UAV_RWBYTEADDRESS:
            m_storageBlocks[name] = StorageSlot{binding.BindPoint, true};
            break;
        case D3D_SIT_CBUFFER:
            m_constantBuffers[name] = binding.BindPoint;
            break;
        default:
            break;
        }
    }
}

ShaderResourceBindingsPtr D3D11ComputeShaderProgram::VCreateComputeResourceBindingObjects()
{
    FN("D3D11ComputeShaderProgram::VCreateComputeResourceBindingObjects");

    return D3D11ShaderResourceBindingsPtr(new D3D11ShaderResourceBindings());
}

void D3D11ComputeShaderProgram::VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    FN("D3D11ComputeShaderProgram::VDispatch");
    OPTICK_EVENT();

    if (!m_ready)
    {
        return;
    }

    m_context->CSSetShader(m_computeShader.Get(), nullptr, 0);

    D3D11ShaderResourceBindingsPtr bindings = std::dynamic_pointer_cast<D3D11ShaderResourceBindings>(shaderResourceBindings);
    if (bindings != nullptr)
    {
        for (const auto &entry : bindings->GetStorageBuffers())
        {
            std::unordered_map<std::string, StorageSlot>::const_iterator slot = m_storageBlocks.find(entry.first);
            if (slot == m_storageBlocks.end())
            {
                LOG_WARNING("Compute shader has no storage block named '%s'; the binding is ignored.", entry.first.c_str());
                continue;
            }

            if (slot->second.writable)
            {
                ID3D11UnorderedAccessView *views[] = {entry.second->GetUnorderedAccessView()};
                m_context->CSSetUnorderedAccessViews(slot->second.slot, 1, views, nullptr);
            }
            else
            {
                ID3D11ShaderResourceView *views[] = {entry.second->GetShaderResourceView()};
                m_context->CSSetShaderResources(slot->second.slot, 1, views);
            }
        }
    }

    m_context->Dispatch(groupCountX, groupCountY, groupCountZ);

    // Unbind the unordered access views: leaving a resource bound for writing
    // stops it from being read anywhere else, and the caller reads the result
    // back straight after this.
    if (bindings != nullptr)
    {
        for (const auto &entry : bindings->GetStorageBuffers())
        {
            std::unordered_map<std::string, StorageSlot>::const_iterator slot = m_storageBlocks.find(entry.first);
            if (slot != m_storageBlocks.end() && slot->second.writable)
            {
                ID3D11UnorderedAccessView *none[] = {nullptr};
                m_context->CSSetUnorderedAccessViews(slot->second.slot, 1, none, nullptr);
            }
        }
    }

    m_context->CSSetShader(nullptr, nullptr, 0);
}

} // namespace bow
