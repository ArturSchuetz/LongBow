#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/IBowRenderDevice.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

namespace bow
{

//! DirectX 11 implementation of IRenderDevice.
/*!
    One ID3D11Device and its immediate context are shared by every window this
    device creates, which is what lets resources be used across them.
*/
class D3D11RenderDevice : public IRenderDevice
{
  public:
    D3D11RenderDevice();
    ~D3D11RenderDevice();

    bool Initialize(uint32_t deviceHandle);
    void VRelease() override;

    GraphicsWindowPtr VCreateWindow(int width, int height, const std::string &title, WindowType type) override;

    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragementShaderFilename) override;
    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &GeometryShaderFilename, const std::string &FragementShaderFilename) override;
    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &TessControlShaderFilename, const std::string &TessEvalShaderFilename) override;

    ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &GeometryShaderFilename, const std::string &TessControlShaderFilename,
                                                  const std::string &TessEvalShaderFilename) override;

    ComputeShaderProgramPtr VCreateComputeShaderProgram(const std::string &computeShaderSource) override;

    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragementShaderSource) override;
    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &GeometryShaderSource, const std::string &FragementShaderSource) override;
    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource) override;

    ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                          const std::string &TessEvalShaderSource) override;

    std::unique_ptr<IRayTracingShaderProgram> VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                            const std::string &intersectionShaderSource, const std::string &callableShaderSource) override;

    MeshBufferPtr VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) override;
    VertexBufferPtr VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes, bool useForRayTracing = false) override;
    IndexBufferPtr VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes, bool useForRayTracing = false) override;
    WritePixelBufferPtr VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) override;
    UniformBufferPtr VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data = nullptr) override;
    StorageBufferPtr VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data = nullptr) override;

    Texture2DPtr VCreateTexture2D(Texture2DDescription description) override;
    Texture2DPtr VCreateTexture2D(ImagePtr image) override;
    TextureSamplerPtr VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1) override;

    std::unique_ptr<IBottomLevelAccelerationStructure> VCreateBottomLevelAccelerationStructure(MeshPtr mesh) override;
    std::unique_ptr<ITopLevelAccelerationStructure> VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures) override;

    ID3D11Device *GetDevice() { return m_device.Get(); }
    ID3D11DeviceContext *GetDeviceContext() { return m_deviceContext.Get(); }
    IDXGIFactory2 *GetFactory() { return m_factory.Get(); }

  private:
    // You shall not copy
    D3D11RenderDevice(const D3D11RenderDevice &) = delete;
    D3D11RenderDevice &operator=(const D3D11RenderDevice &) = delete;

    Microsoft::WRL::ComPtr<IDXGIFactory2> m_factory;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;

    D3D_FEATURE_LEVEL m_featureLevel;
    bool m_initialized;
};

} // namespace bow
