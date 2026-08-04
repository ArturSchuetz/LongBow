#include <DirectX11RenderDevice/Device/Buffer/BowD3D11VertexBuffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11VertexBuffer::D3D11VertexBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, int64_t sizeInBytes) : m_buffer(device, context, D3D11_BIND_VERTEX_BUFFER, usageHint, sizeInBytes)
{
    FN("D3D11VertexBuffer::D3D11VertexBuffer");
}

D3D11VertexBuffer::~D3D11VertexBuffer() { FN("D3D11VertexBuffer::~D3D11VertexBuffer"); }

void D3D11VertexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t lengthInBytes) { m_buffer.CopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes); }

void D3D11VertexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) { m_buffer.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes); }

int64_t D3D11VertexBuffer::VGetSizeInBytes() { return m_buffer.GetSizeInBytes(); }

BufferHint D3D11VertexBuffer::VGetUsageHint() { return m_buffer.GetUsageHint(); }

} // namespace bow
