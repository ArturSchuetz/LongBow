#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>
#include <RenderDevice/Device/Buffer/IBowStorageBuffer.h>

#include <d3d11.h>
#include <wrl/client.h>

namespace bow
{

//! What DirectX 11 offers instead of a shader storage buffer.
/*!
    A GLSL storage block becomes a structured buffer here, read through a
    shader resource view and written through an unordered access view. Both are
    created up front because a compute shader may want either, and which one it
    uses is decided by how the shader declared the block, not by the caller.

    Unlike the other buffers this one cannot be dynamic: a resource the GPU
    writes through a UAV has to live in default memory, so uploads always go
    through UpdateSubresource and read-back through a staging copy.
*/
class D3D11StorageBuffer : public IStorageBuffer
{
  public:
    D3D11StorageBuffer(ID3D11Device *device, ID3D11DeviceContext *context, BufferHint usageHint, int64_t sizeInBytes, const void *initialData);
    ~D3D11StorageBuffer();

    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t lengthInBytes) override;
    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;
    std::shared_ptr<void> VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) override;

    int64_t VGetSizeInBytes() override;
    BufferHint VGetUsageHint() override;

    ID3D11ShaderResourceView *GetShaderResourceView() const { return m_shaderResourceView.Get(); }
    ID3D11UnorderedAccessView *GetUnorderedAccessView() const { return m_unorderedAccessView.Get(); }

  private:
    D3D11StorageBuffer(const D3D11StorageBuffer &) = delete;
    D3D11StorageBuffer &operator=(const D3D11StorageBuffer &) = delete;

    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_unorderedAccessView;

    int64_t m_sizeInBytes;
    BufferHint m_usageHint;
};

typedef std::shared_ptr<D3D11StorageBuffer> D3D11StorageBufferPtr;

} // namespace bow
