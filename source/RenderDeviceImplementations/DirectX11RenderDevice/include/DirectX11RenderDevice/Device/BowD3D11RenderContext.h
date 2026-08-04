#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/IBowRenderContext.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

namespace bow
{

//! Render context for one DirectX 11 swap chain.
/*!
    DirectX 11 has an immediate context that owns the pipeline state, so unlike
    the OpenGL backend there is no global state to make current -- every call
    goes through the device context held here.
*/
class D3D11RenderContext : public IRenderContext
{
  public:
    D3D11RenderContext(HWND windowHandle, uint32_t width, uint32_t height);
    ~D3D11RenderContext();

    bool Initialize(D3D11RenderDevice *device);
    void VRelease() override;

    VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) override;
    VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers) override;
    VertexAttributeBindingsPtr VCreateVertexAttributeBindings() override;
    FramebufferPtr VCreateFramebuffer() override;

    void VBeginFrame() override;
    void VEndFrame() override;
    void VClear(ClearState clearState) override;

    void VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end) override;

    void VSetFramebuffer(FramebufferPtr framebufer) override;
    void VSetViewport(Viewport viewport) override;
    Viewport VGetViewport() override;

    void VSwapBuffers(bool vsync = false) override;

    void VTraceRays(void *shaderProgram, ShaderResourceBindingsPtr resourceBindings, Texture2DPtr outputImage, uint32_t width, uint32_t height) override;

    //! Recreates the back buffer views after the window changed size.
    void Resize(uint32_t width, uint32_t height);

    ID3D11Device *GetDevice() { return m_device.Get(); }
    ID3D11DeviceContext *GetDeviceContext() { return m_deviceContext.Get(); }

  private:
    // You shall not copy
    D3D11RenderContext(const D3D11RenderContext &) = delete;
    D3D11RenderContext &operator=(const D3D11RenderContext &) = delete;

    bool CreateBackBufferViews();
    void ReleaseBackBufferViews();

    //! Sets everything a draw needs and issues it.
    void Draw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState);

    //! Applies a render state, creating and caching the state objects it needs.
    /*!
        DirectX 11 wants rasterizer, blend and depth-stencil settings as objects
        created ahead of time, where OpenGL takes individual calls. Building one
        per draw would be wasteful, so they are cached by the settings they were
        built from.
    */
    void ApplyRenderState(const RenderState &renderState);

    //! Resolves the names in the bindings to slots using the shader reflection.
    void ApplyShaderResourceBindings(ShaderResourceBindingsPtr shaderResourceBindings, const std::shared_ptr<class D3D11ShaderProgram> &program);

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    //! What the cached state objects were built from, so they can be reused.
    uint64_t m_renderStateKey;
    bool m_renderStateValid;

    //! Offscreen target currently bound, or null for the window.
    std::shared_ptr<class D3D11Framebuffer> m_boundFramebuffer;

    HWND m_windowHandle;
    Viewport m_viewport;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;

    bool m_initialized;
};

} // namespace bow
