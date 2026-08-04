#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Shader/IBowComputeShaderProgram.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <string>
#include <unordered_map>

namespace bow
{

//! A compute shader and the slots its storage blocks were reflected into.
/*!
    GLSL storage blocks arrive as structured buffers. Which view is bound
    depends on how the shader declared the block: a readonly block reflects as
    a shader resource and is bound with CSSetShaderResources, anything writable
    reflects as an unordered access resource and goes to CSSetUnorderedAccessViews.
    The reflection is what decides, so the caller does not have to say.
*/
class D3D11ComputeShaderProgram : public IComputeShaderProgram
{
  public:
    D3D11ComputeShaderProgram(ID3D11Device *device, ID3D11DeviceContext *context, const std::string &computeShaderSource);
    ~D3D11ComputeShaderProgram();

    ShaderResourceBindingsPtr VCreateComputeResourceBindingObjects() override;
    void VDispatch(ShaderResourceBindingsPtr shaderResourceBindings, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    bool IsReady() const { return m_ready; }

  private:
    D3D11ComputeShaderProgram(const D3D11ComputeShaderProgram &) = delete;
    D3D11ComputeShaderProgram &operator=(const D3D11ComputeShaderProgram &) = delete;

    struct StorageSlot
    {
        uint32_t slot;
        //! Writable blocks bind as a UAV, read-only ones as a shader resource.
        bool writable;
    };

    void Reflect(ID3DBlob *byteCode, const std::unordered_map<uint32_t, std::string> &originalNames);

    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;

    std::unordered_map<std::string, StorageSlot> m_storageBlocks;
    std::unordered_map<std::string, uint32_t> m_constantBuffers;

    bool m_ready;
};

typedef std::shared_ptr<D3D11ComputeShaderProgram> D3D11ComputeShaderProgramPtr;

} // namespace bow
