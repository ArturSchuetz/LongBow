#pragma once
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11Buffer.h>

#include <RenderDevice/Device/Buffer/IBowVertexBuffer.h>

namespace bow
{

class D3D11VertexBuffer : public IVertexBuffer
{
  public:
    D3D11VertexBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, int64_t sizeInBytes);
    ~D3D11VertexBuffer();

    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t lengthInBytes) override;
    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    ID3D11Buffer *GetHandle() const { return m_buffer.GetHandle(); }

  private:
    D3D11VertexBuffer(const D3D11VertexBuffer &) = delete;
    D3D11VertexBuffer &operator=(const D3D11VertexBuffer &) = delete;

    D3D11Buffer m_buffer;
};

typedef std::shared_ptr<D3D11VertexBuffer> D3D11VertexBufferPtr;

} // namespace bow
