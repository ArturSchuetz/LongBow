#include <DirectX11RenderDevice/Device/Shader/BowD3D11ShaderResourceBindings.h>

#include <DirectX11RenderDevice/Device/Buffer/BowD3D11UniformBuffer.h>
#include <DirectX11RenderDevice/Device/Textures/BowD3D11Texture2D.h>
#include <DirectX11RenderDevice/Device/Textures/BowD3D11TextureSampler.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11ShaderResourceBindings::D3D11ShaderResourceBindings() : m_textures(), m_uniformBuffers() {}

D3D11ShaderResourceBindings::~D3D11ShaderResourceBindings() {}

void D3D11ShaderResourceBindings::VSetBuffer(const char *name, UniformBufferPtr uniformBuffer)
{
    FN("D3D11ShaderResourceBindings::VSetBuffer");

    D3D11UniformBufferPtr buffer = std::dynamic_pointer_cast<D3D11UniformBuffer>(uniformBuffer);
    if (buffer == nullptr)
    {
        LOG_ERROR("Uniform buffer bound to '%s' was not created by the DirectX 11 device.", name);
        return;
    }

    m_uniformBuffers[name] = buffer;
}

void D3D11ShaderResourceBindings::VSetBuffer(const char *name, StorageBufferPtr /*storageBuffer*/)
{
    FN("D3D11ShaderResourceBindings::VSetBuffer");

    LOG_ERROR("DirectX 11: storage buffers are not implemented yet, ignoring '%s'.", name);
}

void D3D11ShaderResourceBindings::VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler)
{
    FN("D3D11ShaderResourceBindings::VSetTexture");

    D3D11Texture2DPtr d3dTexture = std::dynamic_pointer_cast<D3D11Texture2D>(texture);
    if (d3dTexture == nullptr)
    {
        LOG_ERROR("Texture bound to '%s' was not created by the DirectX 11 device.", name);
        return;
    }

    TextureBinding binding;
    binding.texture = d3dTexture;
    binding.sampler = std::dynamic_pointer_cast<D3D11TextureSampler>(sampler);

    m_textures[name] = binding;
}

void D3D11ShaderResourceBindings::VSetAccelerationStructure(const char *name, void *)
{
    FN("D3D11ShaderResourceBindings::VSetAccelerationStructure");

    LOG_ERROR("Ray tracing is not supported in DirectX 11, ignoring acceleration structure '%s'.", name);
}

void D3D11ShaderResourceBindings::VSetStorageImage(const char *name, Texture2DPtr)
{
    FN("D3D11ShaderResourceBindings::VSetStorageImage");

    LOG_ERROR("DirectX 11: storage images are not implemented yet, ignoring '%s'.", name);
}

} // namespace bow
