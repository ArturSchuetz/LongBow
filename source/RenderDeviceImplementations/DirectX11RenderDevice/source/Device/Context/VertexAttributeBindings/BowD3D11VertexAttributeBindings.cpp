#include <DirectX11RenderDevice/Device/Context/VertexAttributeBindings/BowD3D11VertexAttributeBindings.h>

#include <DirectX11RenderDevice/BowD3D11TypeConverter.h>
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11IndexBuffer.h>
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11VertexBuffer.h>
#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderProgram.h>

#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11VertexAttributeBindings::D3D11VertexAttributeBindings(ID3D11Device *device, ID3D11DeviceContext *context) : m_device(device), m_context(context), m_layoutProgram(nullptr), m_dirty(true)
{
    FN("D3D11VertexAttributeBindings::D3D11VertexAttributeBindings");
}

D3D11VertexAttributeBindings::~D3D11VertexAttributeBindings() { FN("D3D11VertexAttributeBindings::~D3D11VertexAttributeBindings"); }

VertexBufferAttributeMap D3D11VertexAttributeBindings::VGetAttributes() { return m_attributes; }

void D3D11VertexAttributeBindings::VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer)
{
    FN("D3D11VertexAttributeBindings::VSetAttribute");

    m_attributes[location] = pointer;
    m_dirty = true;
}

void D3D11VertexAttributeBindings::VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer)
{
    FN("D3D11VertexAttributeBindings::VSetAttribute");

    if (vertexAttribute == nullptr)
    {
        // The example asked for an attribute the shader does not declare. That
        // is worth reporting rather than silently drawing without it.
        LOG_ERROR("Cannot bind a buffer to an attribute the shader does not have.");
        return;
    }

    VSetAttribute((uint32_t)vertexAttribute->Location, pointer);
}

IndexBufferPtr D3D11VertexAttributeBindings::VGetIndexBuffer() { return m_indexBuffer; }

void D3D11VertexAttributeBindings::VSetIndexBuffer(IndexBufferPtr pointer)
{
    FN("D3D11VertexAttributeBindings::VSetIndexBuffer");

    m_indexBuffer = pointer;
}

uint32_t D3D11VertexAttributeBindings::GetVertexCount() const
{
    FN("D3D11VertexAttributeBindings::GetVertexCount");

    // Every attribute covers the same vertices, so the shortest one decides how
    // many can be drawn.
    uint32_t count = 0;
    for (auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
    {
        if (it->second == nullptr || it->second->GetVertexBuffer() == nullptr)
        {
            continue;
        }

        const int stride = it->second->GetStrideInBytes();
        if (stride <= 0)
        {
            continue;
        }

        const uint32_t forThisAttribute = (uint32_t)(it->second->GetVertexBuffer()->VGetSizeInBytes() / stride);
        if (count == 0 || forThisAttribute < count)
        {
            count = forThisAttribute;
        }
    }
    return count;
}

bool D3D11VertexAttributeBindings::EnsureInputLayout(const D3D11ShaderProgramPtr &program)
{
    FN("D3D11VertexAttributeBindings::EnsureInputLayout");

    if (m_inputLayout != nullptr && !m_dirty && m_layoutProgram == program.get())
    {
        return true;
    }

    ID3DBlob *byteCode = program->GetVertexByteCode();
    if (byteCode == nullptr)
    {
        LOG_ERROR("The shader program has no vertex bytecode to validate an input layout against.");
        return false;
    }

    // The semantic has to be the one the shader declares. Translated shaders
    // come out of SPIRV-Cross with TEXCOORD<location>, so the attribute's
    // location is the semantic index and the name is always TEXCOORD.
    std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
    elements.reserve(m_attributes.size());

    UINT slot = 0;
    for (auto it = m_attributes.begin(); it != m_attributes.end(); ++it, ++slot)
    {
        if (it->second == nullptr)
        {
            continue;
        }

        D3D11_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = "TEXCOORD";
        element.SemanticIndex = (UINT)it->first;
        element.Format = D3D11TypeConverter::ToDXGIFormat(it->second->GetComponentDatatype(), it->second->GetNumberOfComponents());
        // One buffer per attribute means one input slot per attribute, and the
        // offset within that slot is therefore always zero.
        element.InputSlot = slot;
        element.AlignedByteOffset = 0;
        element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        elements.push_back(element);
    }

    if (elements.empty())
    {
        LOG_ERROR("No vertex attributes were set, so there is nothing to build an input layout from.");
        return false;
    }

    m_inputLayout.Reset();
    const HRESULT result = m_device->CreateInputLayout(elements.data(), (UINT)elements.size(), byteCode->GetBufferPointer(), byteCode->GetBufferSize(), &m_inputLayout);
    if (FAILED(result))
    {
        LOG_ERROR("CreateInputLayout failed (0x%08X). The attributes set do not match what the vertex shader declares.", (unsigned)result);
        return false;
    }

    m_layoutProgram = program.get();
    m_dirty = false;
    return true;
}

bool D3D11VertexAttributeBindings::Bind(const D3D11ShaderProgramPtr &program)
{
    FN("D3D11VertexAttributeBindings::Bind");

    if (program == nullptr || !EnsureInputLayout(program))
    {
        return false;
    }

    m_context->IASetInputLayout(m_inputLayout.Get());

    std::vector<ID3D11Buffer *> buffers;
    std::vector<UINT> strides;
    std::vector<UINT> offsets;
    buffers.reserve(m_attributes.size());
    strides.reserve(m_attributes.size());
    offsets.reserve(m_attributes.size());

    for (auto it = m_attributes.begin(); it != m_attributes.end(); ++it)
    {
        if (it->second == nullptr)
        {
            continue;
        }

        D3D11VertexBufferPtr vertexBuffer = std::dynamic_pointer_cast<D3D11VertexBuffer>(it->second->GetVertexBuffer());
        if (vertexBuffer == nullptr)
        {
            LOG_ERROR("A vertex buffer bound here was not created by the DirectX 11 device.");
            return false;
        }

        buffers.push_back(vertexBuffer->GetHandle());
        strides.push_back((UINT)it->second->GetStrideInBytes());
        offsets.push_back((UINT)it->second->GetOffsetInBytes());
    }

    if (buffers.empty())
    {
        return false;
    }

    m_context->IASetVertexBuffers(0, (UINT)buffers.size(), buffers.data(), strides.data(), offsets.data());

    if (m_indexBuffer != nullptr)
    {
        D3D11IndexBufferPtr indexBuffer = std::dynamic_pointer_cast<D3D11IndexBuffer>(m_indexBuffer);
        if (indexBuffer != nullptr)
        {
            const DXGI_FORMAT format = (indexBuffer->GetDatatype() == IndexBufferDatatype::UnsignedInt16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
            m_context->IASetIndexBuffer(indexBuffer->GetHandle(), format, 0);
        }
    }

    return true;
}

} // namespace bow
