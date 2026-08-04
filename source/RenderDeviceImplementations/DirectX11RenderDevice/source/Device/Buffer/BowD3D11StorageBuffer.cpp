#include <DirectX11RenderDevice/Device/Buffer/BowD3D11StorageBuffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11StorageBuffer::D3D11StorageBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, int64_t sizeInBytes, const void *initialData)
    : m_device(device), m_context(context), m_sizeInBytes(sizeInBytes), m_usageHint(usageHint)
{
    FN("D3D11StorageBuffer::D3D11StorageBuffer");

    LOG_ASSERT(!(sizeInBytes <= 0), "sizeInBytes must be greater than zero.");

    // Four bytes per element, matching the float and uint storage blocks the
    // examples declare. A structured buffer needs a stride, and the views are
    // sized from it.
    const UINT elementStride = 4;
    const UINT elementCount = (UINT)((sizeInBytes + elementStride - 1) / elementStride);

    D3D11_BUFFER_DESC description = {};
    description.ByteWidth = elementCount * elementStride;
    description.Usage = D3D11_USAGE_DEFAULT;
    description.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    description.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    description.StructureByteStride = elementStride;

    D3D11_SUBRESOURCE_DATA initial = {};
    initial.pSysMem = initialData;

    HRESULT result = device->CreateBuffer(&description, initialData != nullptr ? &initial : nullptr, &m_buffer);
    if (FAILED(result))
    {
        LOG_ERROR("Could not create a structured buffer of %lld bytes (0x%08X).", (long long)sizeInBytes, (unsigned)result);
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceView = {};
    shaderResourceView.Format = DXGI_FORMAT_UNKNOWN;
    shaderResourceView.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shaderResourceView.Buffer.NumElements = elementCount;

    result = device->CreateShaderResourceView(m_buffer.Get(), &shaderResourceView, &m_shaderResourceView);
    if (FAILED(result))
    {
        LOG_ERROR("Could not create a shader resource view for the storage buffer (0x%08X).", (unsigned)result);
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessView = {};
    unorderedAccessView.Format = DXGI_FORMAT_UNKNOWN;
    unorderedAccessView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    unorderedAccessView.Buffer.NumElements = elementCount;

    result = device->CreateUnorderedAccessView(m_buffer.Get(), &unorderedAccessView, &m_unorderedAccessView);
    if (FAILED(result))
    {
        LOG_ERROR("Could not create an unordered access view for the storage buffer (0x%08X).", (unsigned)result);
    }
}

D3D11StorageBuffer::~D3D11StorageBuffer() { FN("D3D11StorageBuffer::~D3D11StorageBuffer"); }

void D3D11StorageBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t lengthInBytes) { VCopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes); }

void D3D11StorageBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("D3D11StorageBuffer::VCopyFromSystemMemory");

    if (m_buffer == nullptr || bufferInSystemMemory == nullptr)
    {
        return;
    }

    D3D11_BOX box = {};
    box.left = (UINT)destinationOffsetInBytes;
    box.right = (UINT)(destinationOffsetInBytes + lengthInBytes);
    box.bottom = 1;
    box.back = 1;

    m_context->UpdateSubresource(m_buffer.Get(), 0, &box, bufferInSystemMemory, 0, 0);
}

std::shared_ptr<void> D3D11StorageBuffer::VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes)
{
    FN("D3D11StorageBuffer::VCopyToSystemMemory");

    if (m_buffer == nullptr)
    {
        return nullptr;
    }

    // The compute result lives in GPU memory the CPU cannot touch, so it is
    // copied into a staging buffer first. This waits for the dispatch to
    // finish, which is what a caller reading results back wants anyway.
    D3D11_BUFFER_DESC description = {};
    m_buffer->GetDesc(&description);
    description.Usage = D3D11_USAGE_STAGING;
    description.BindFlags = 0;
    description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    description.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    Microsoft::WRL::ComPtr<ID3D11Buffer> staging;
    HRESULT result = m_device->CreateBuffer(&description, nullptr, &staging);
    if (FAILED(result))
    {
        LOG_ERROR("Could not create a staging buffer to read the storage buffer back (0x%08X).", (unsigned)result);
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

    unsigned char *copy = new unsigned char[sizeInBytes];
    memcpy(copy, static_cast<const char *>(mapped.pData) + offsetInBytes, (size_t)sizeInBytes);
    m_context->Unmap(staging.Get(), 0);

    return std::shared_ptr<void>(copy, [](void *ptr) { delete[] static_cast<unsigned char *>(ptr); });
}

int64_t D3D11StorageBuffer::VGetSizeInBytes() { return m_sizeInBytes; }

BufferHint D3D11StorageBuffer::VGetUsageHint() { return m_usageHint; }

} // namespace bow
