#pragma once
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11Buffer.h>

#include <RenderDevice/Device/Buffer/BowIndexBufferDatatype.h>
#include <RenderDevice/Device/Buffer/IBowIndexBuffer.h>

namespace bow
{

class D3D11IndexBuffer : public IIndexBuffer
{
  public:
    D3D11IndexBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes);
    ~D3D11IndexBuffer();

    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t lengthInBytes) override;
    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    ID3D11Buffer *GetHandle() const { return m_buffer.GetHandle(); }
    IndexBufferDatatype GetDatatype() const { return m_dataType; }

    //! Number of indices the buffer holds, from its size and index width.
    int GetCount() const;

  private:
    D3D11IndexBuffer(const D3D11IndexBuffer &) = delete;
    D3D11IndexBuffer &operator=(const D3D11IndexBuffer &) = delete;

    D3D11Buffer m_buffer;
    IndexBufferDatatype m_dataType;
};

typedef std::shared_ptr<D3D11IndexBuffer> D3D11IndexBufferPtr;

} // namespace bow
