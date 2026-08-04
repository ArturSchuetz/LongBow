#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>
#include <RenderDevice/Device/Shader/IBowShaderProgram.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace bow
{

//! A vertex and pixel shader pair, plus what reflection says about them.
/*!
    Where OpenGL links shaders into one program object, DirectX 11 keeps the
    stages separate and binds them individually. This class is the pair, so the
    rest of the backend can go on treating a shader program as one thing.

    The compiled vertex bytecode is kept because an input layout can only be
    created against it -- DirectX validates the layout against the signature
    the shader actually declares.
*/
class D3D11ShaderProgram : public IShaderProgram
{
  public:
    //! Where a named resource ends up in the DirectX 11 binding model.
    struct ResourceSlot
    {
        uint32_t slot;
        bool inVertexStage;
        bool inPixelStage;
    };

    D3D11ShaderProgram(ID3D11Device *device, ID3D11DeviceContext *context, const std::string &vertexSource, const std::string &fragmentSource, const std::string &geometrySource);
    ~D3D11ShaderProgram();

    ShaderVertexAttributePtr VGetVertexAttribute(std::string name) override;
    ShaderVertexAttributeMap VGetVertexAttributes() override;
    int VGetFragmentOutputLocation(std::string name) override;

    ShaderResourceBindingsPtr VCreateResourceBindingObjects() override;

    void VSetPushConstants(const char *name, const void *data, size_t offset, size_t size) override;
    void VSetPushConstants(ShaderStage shaderStage, const void *data, size_t offset, size_t size) override;

    bool IsReady() const { return m_ready; }

    //! Binds both stages on the immediate context.
    void Bind();

    ID3DBlob *GetVertexByteCode() const { return m_vertexByteCode.Get(); }

    //! Slot a named constant buffer, texture or sampler was reflected into.
    const ResourceSlot *FindConstantBuffer(const std::string &name) const;
    const ResourceSlot *FindTexture(const std::string &name) const;
    const ResourceSlot *FindSampler(const std::string &name) const;

  private:
    // You shall not copy
    D3D11ShaderProgram(const D3D11ShaderProgram &) = delete;
    D3D11ShaderProgram &operator=(const D3D11ShaderProgram &) = delete;

    bool CompileStage(const std::string &source, const char *target, Microsoft::WRL::ComPtr<ID3DBlob> &byteCode);
    void ReflectVertexInput(ID3DBlob *byteCode, const std::unordered_map<uint32_t, std::string> &originalNames);
    void ReflectResources(ID3DBlob *byteCode, bool vertexStage);

    ID3D11Device *m_device;
    ID3D11DeviceContext *m_context;

    Microsoft::WRL::ComPtr<ID3DBlob> m_vertexByteCode;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

    ShaderVertexAttributeMap m_vertexAttributes;

    std::unordered_map<std::string, ResourceSlot> m_constantBuffers;
    std::unordered_map<std::string, ResourceSlot> m_textures;
    std::unordered_map<std::string, ResourceSlot> m_samplers;

    //! Constant buffer standing in for a push-constant block, by block name.
    struct PushConstantBlock
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
        uint32_t slot;
        size_t sizeInBytes;
        bool inVertexStage;
        bool inPixelStage;
    };
    std::unordered_map<std::string, PushConstantBlock> m_pushConstantBlocks;

    bool m_ready;
};

typedef std::shared_ptr<D3D11ShaderProgram> D3D11ShaderProgramPtr;

} // namespace bow
