#include <DirectX11RenderDevice/Device/Buffer/BowD3D11UniformBuffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11UniformBuffer::D3D11UniformBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, int64_t sizeInBytes, const void *initialData)
    : m_buffer(device, context, D3D11_BIND_CONSTANT_BUFFER, usageHint, sizeInBytes, initialData)
{
    FN("D3D11UniformBuffer::D3D11UniformBuffer");
}

D3D11UniformBuffer::~D3D11UniformBuffer() { FN("D3D11UniformBuffer::~D3D11UniformBuffer"); }

void D3D11UniformBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t lengthInBytes) { m_buffer.CopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes); }

void D3D11UniformBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) { m_buffer.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes); }

std::shared_ptr<void> D3D11UniformBuffer::VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) { return m_buffer.CopyToSystemMemory(offsetInBytes, sizeInBytes); }

int64_t D3D11UniformBuffer::VGetSizeInBytes() { return m_buffer.GetSizeInBytes(); }

BufferHint D3D11UniformBuffer::VGetUsageHint() { return m_buffer.GetUsageHint(); }

} // namespace bow
