#include <DirectX11RenderDevice/Device/Buffer/BowD3D11Buffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{
namespace
{

//! Whether the hint says the buffer will be rewritten by the CPU.
/*!
    The Draw/Read/Copy part of the hint describes who touches the data, and the
    Static/Dynamic/Stream part how often. Anything not static is mapped rather
    than updated through the command list.
*/
bool IsDynamicHint(BufferHint hint)
{
    switch (hint)
    {
    case BufferHint::StreamDraw:
    case BufferHint::StreamRead:
    case BufferHint::StreamCopy:
    case BufferHint::DynamicDraw:
    case BufferHint::DynamicRead:
    case BufferHint::DynamicCopy:
        return true;
    default:
        return false;
    }
}

} // namespace

D3D11Buffer::D3D11Buffer(ID3D11Device *device, ID3D11DeviceContext *context, UINT bindFlags, BufferHint usageHint, int64_t sizeInBytes, const void *initialData)
    : m_device(device), m_context(context), m_sizeInBytes(sizeInBytes), m_usageHint(usageHint), m_dynamic(IsDynamicHint(usageHint))
{
    FN("D3D11Buffer::D3D11Buffer");

    LOG_ASSERT(!(sizeInBytes <= 0), "sizeInBytes must be greater than zero.");

    D3D11_BUFFER_DESC description = {};
    // Constant buffers are read in 16-byte registers, and creating one whose
    // size is not a multiple of that is rejected outright.
    if ((bindFlags & D3D11_BIND_CONSTANT_BUFFER) != 0)
    {
        m_sizeInBytes = (sizeInBytes + 15) & ~static_cast<int64_t>(15);
    }

    description.ByteWidth = (UINT)m_sizeInBytes;
    description.BindFlags = bindFlags;

    if (m_dynamic)
    {
        description.Usage = D3D11_USAGE_DYNAMIC;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else
    {
        description.Usage = D3D11_USAGE_DEFAULT;
    }

    D3D11_SUBRESOURCE_DATA initial = {};
    initial.pSysMem = initialData;

    const HRESULT result = device->CreateBuffer(&description, initialData != nullptr ? &initial : nullptr, &m_buffer);
    if (FAILED(result))
    {
        LOG_ERROR("CreateBuffer failed for %lld bytes (0x%08X).", (long long)m_sizeInBytes, (unsigned)result);
    }
}

D3D11Buffer::~D3D11Buffer() { FN("D3D11Buffer::~D3D11Buffer"); }

void D3D11Buffer::CopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("D3D11Buffer::CopyFromSystemMemory");

    if (m_buffer == nullptr || bufferInSystemMemory == nullptr)
    {
        return;
    }

    LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or equal to the buffer size.");

    if (m_dynamic)
    {
        // MAP_WRITE_DISCARD replaces the whole buffer, so a partial update has
        // to read the rest back from somewhere. Rather than keep a shadow copy,
        // a partial write to a dynamic buffer uses NO_OVERWRITE, which promises
        // the range being written is not in flight.
        const D3D11_MAP mapType = (destinationOffsetInBytes == 0 && lengthInBytes == m_sizeInBytes) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;

        D3D11_MAPPED_SUBRESOURCE mapped = {};
        const HRESULT result = m_context->Map(m_buffer.Get(), 0, mapType, 0, &mapped);
        if (FAILED(result))
        {
            LOG_ERROR("Map failed (0x%08X).", (unsigned)result);
            return;
        }

        memcpy(static_cast<char *>(mapped.pData) + destinationOffsetInBytes, bufferInSystemMemory, (size_t)lengthInBytes);
        m_context->Unmap(m_buffer.Get(), 0);
        return;
    }

    D3D11_BOX box = {};
    box.left = (UINT)destinationOffsetInBytes;
    box.right = (UINT)(destinationOffsetInBytes + lengthInBytes);
    box.bottom = 1;
    box.back = 1;

    m_context->UpdateSubresource(m_buffer.Get(), 0, &box, bufferInSystemMemory, 0, 0);
}

std::shared_ptr<void> D3D11Buffer::CopyToSystemMemory(int64_t offsetInBytes, int64_t lengthInBytes)
{
    FN("D3D11Buffer::CopyToSystemMemory");

    if (m_buffer == nullptr)
    {
        return nullptr;
    }

    LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(offsetInBytes + lengthInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to the buffer size.");

    // A buffer the GPU renders from cannot be read by the CPU, so the read goes
    // through a staging copy. This stalls until the copy is done and is meant
    // for tools and tests rather than for a frame.
    D3D11_BUFFER_DESC description = {};
    m_buffer->GetDesc(&description);
    description.Usage = D3D11_USAGE_STAGING;
    description.BindFlags = 0;
    description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    description.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> staging;
    HRESULT result = m_device->CreateBuffer(&description, nullptr, &staging);
    if (FAILED(result))
    {
        LOG_ERROR("Could not create a staging buffer for read-back (0x%08X).", (unsigned)result);
        return nullptr;
    }

    m_context->CopyResource(staging.Get(), m_buffer.Get());

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    result = m_context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(result))
    {
        LOG_ERROR("Map for read-back failed (0x%08X).", (unsigned)result);
        return nullptr;
    }

    unsigned char *copy = new unsigned char[lengthInBytes];
    memcpy(copy, static_cast<const char *>(mapped.pData) + offsetInBytes, (size_t)lengthInBytes);
    m_context->Unmap(staging.Get(), 0);

    return std::shared_ptr<void>(copy, [](void *ptr) { delete[] static_cast<unsigned char *>(ptr); });
}

} // namespace bow
