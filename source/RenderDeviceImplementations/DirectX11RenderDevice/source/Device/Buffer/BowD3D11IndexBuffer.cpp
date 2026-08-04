#include <DirectX11RenderDevice/Device/Buffer/BowD3D11IndexBuffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11IndexBuffer::D3D11IndexBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes)
    : m_buffer(device, context, D3D11_BIND_INDEX_BUFFER, usageHint, sizeInBytes), m_dataType(dataType)
{
    FN("D3D11IndexBuffer::D3D11IndexBuffer");
}

D3D11IndexBuffer::~D3D11IndexBuffer() { FN("D3D11IndexBuffer::~D3D11IndexBuffer"); }

void D3D11IndexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t lengthInBytes) { m_buffer.CopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes); }

void D3D11IndexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) { m_buffer.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes); }

int64_t D3D11IndexBuffer::VGetSizeInBytes() { return m_buffer.GetSizeInBytes(); }

BufferHint D3D11IndexBuffer::VGetUsageHint() { return m_buffer.GetUsageHint(); }

int D3D11IndexBuffer::GetCount() const
{
    FN("D3D11IndexBuffer::GetCount");

    const int64_t stride = (m_dataType == IndexBufferDatatype::UnsignedInt16) ? 2 : 4;
    return (int)(m_buffer.GetSizeInBytes() / stride);
}

} // namespace bow
