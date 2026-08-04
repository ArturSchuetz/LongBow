#pragma once
#include <DirectX11RenderDevice/Device/Buffer/BowD3D11Buffer.h>

#include <RenderDevice/Device/Buffer/IBowUniformBuffer.h>

namespace bow
{

//! A constant buffer, which is what DirectX 11 calls a uniform buffer.
class D3D11UniformBuffer : public IUniformBuffer
{
  public:
    D3D11UniformBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, int64_t sizeInBytes, const void *initialData);
    ~D3D11UniformBuffer();

    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t lengthInBytes) override;
    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;
    std::shared_ptr<void> VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) override;

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    ID3D11Buffer *GetHandle() const { return m_buffer.GetHandle(); }

  private:
    D3D11UniformBuffer(const D3D11UniformBuffer &) = delete;
    D3D11UniformBuffer &operator=(const D3D11UniformBuffer &) = delete;

    D3D11Buffer m_buffer;
};

typedef std::shared_ptr<D3D11UniformBuffer> D3D11UniformBufferPtr;

} // namespace bow
