#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Buffer/BowBufferHint.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

namespace bow
{

//! The ID3D11Buffer behind every buffer type this backend exposes.
/*!
    DirectX 11 has one buffer object with a bind flag saying what it is for,
    where OpenGL has one object bound to different targets. Both mean the same
    thing, so vertex, index and constant buffers all delegate here and differ
    only in the flag they pass.

    The usage hint decides how an update is performed. A dynamic buffer is
    mapped with DISCARD, which hands back fresh memory and never stalls; a
    default buffer goes through UpdateSubresource. Getting that wrong is the
    usual reason a DirectX 11 renderer is slower than it should be, so the
    choice is made once here rather than at every call site.
*/
class D3D11Buffer
{
  public:
    D3D11Buffer(ID3D11Device *device, ID3D11DeviceContext *context, UINT bindFlags, BufferHint usageHint, int64_t sizeInBytes, const void *initialData = nullptr);
    ~D3D11Buffer();

    void CopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes);
    std::shared_ptr<void> CopyToSystemMemory(int64_t offsetInBytes, int64_t lengthInBytes);

    ID3D11Buffer *GetHandle() const { return m_buffer.Get(); }
    int64_t GetSizeInBytes() const { return m_sizeInBytes; }
    BufferHint GetUsageHint() const { return m_usageHint; }
    bool IsReady() const { return m_buffer != nullptr; }

  private:
    // You shall not copy
    D3D11Buffer(const D3D11Buffer &) = delete;
    D3D11Buffer &operator=(const D3D11Buffer &) = delete;

    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

    int64_t m_sizeInBytes;
    BufferHint m_usageHint;
    bool m_dynamic;
};

} // namespace bow
