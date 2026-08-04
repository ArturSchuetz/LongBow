#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Shader/IBowShaderResourceBindings.h>

#include <string>
#include <unordered_map>

namespace bow
{

typedef std::shared_ptr<class D3D11Texture2D> D3D11Texture2DPtr;
typedef std::shared_ptr<class D3D11TextureSampler> D3D11TextureSamplerPtr;
typedef std::shared_ptr<class D3D11UniformBuffer> D3D11UniformBufferPtr;

//! What DirectX 11 has instead of a descriptor set.
/*!
    Like the OpenGL one, this records what the caller asked for by the name
    used in the shader and does not touch the device. The render context turns
    the names into slots using the shader's reflection just before a draw,
    because DirectX 11 binds per stage against the immediate context rather
    than from a prepared table.
*/
class D3D11ShaderResourceBindings : public IShaderResourceBindings
{
  public:
    struct TextureBinding
    {
        D3D11Texture2DPtr texture;
        D3D11TextureSamplerPtr sampler;
    };

    D3D11ShaderResourceBindings();
    ~D3D11ShaderResourceBindings();

    void VSetBuffer(const char *name, UniformBufferPtr uniformBuffer) override;
    void VSetBuffer(const char *name, StorageBufferPtr storageBuffer) override;
    void VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler) override;
    void VSetAccelerationStructure(const char *name, void *accelerationStructure) override;
    void VSetStorageImage(const char *name, Texture2DPtr texture) override;

    const std::unordered_map<std::string, TextureBinding> &GetTextures() const { return m_textures; }
    const std::unordered_map<std::string, D3D11UniformBufferPtr> &GetUniformBuffers() const { return m_uniformBuffers; }

  private:
    D3D11ShaderResourceBindings(const D3D11ShaderResourceBindings &) = delete;
    D3D11ShaderResourceBindings &operator=(const D3D11ShaderResourceBindings &) = delete;

    std::unordered_map<std::string, TextureBinding> m_textures;
    std::unordered_map<std::string, D3D11UniformBufferPtr> m_uniformBuffers;
};

typedef std::shared_ptr<D3D11ShaderResourceBindings> D3D11ShaderResourceBindingsPtr;

} // namespace bow
