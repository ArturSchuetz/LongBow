#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderResourceBindings.h>

#include <OpenGLRenderDevice/Device/Buffer/BowOGLStorageBuffer.h>
#include <OpenGLRenderDevice/Device/Buffer/BowOGLUniformBuffer.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTexture2D.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTextureSampler.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

OGLShaderResourceBindings::OGLShaderResourceBindings() : m_textures(), m_uniformBuffers(), m_storageBuffers(), m_storageImages() {}

OGLShaderResourceBindings::~OGLShaderResourceBindings() {}

void OGLShaderResourceBindings::VSetBuffer(const char *name, UniformBufferPtr uniformBuffer)
{
    FN("OGLShaderResourceBindings::VSetBuffer");

    OGLShaderResourceBufferPtr buffer = std::dynamic_pointer_cast<OGLShaderResourceBuffer>(uniformBuffer);
    if (buffer == nullptr)
    {
        LOG_ERROR("Uniform buffer bound to '%s' was not created by the OpenGL device.", name);
        return;
    }

    m_uniformBuffers[name] = buffer;
}

void OGLShaderResourceBindings::VSetBuffer(const char *name, StorageBufferPtr storageBuffer)
{
    FN("OGLShaderResourceBindings::VSetBuffer");

    OGLStorageBufferPtr buffer = std::dynamic_pointer_cast<OGLStorageBuffer>(storageBuffer);
    if (buffer == nullptr)
    {
        LOG_ERROR("Storage buffer bound to '%s' was not created by the OpenGL device.", name);
        return;
    }

    m_storageBuffers[name] = buffer;
}

void OGLShaderResourceBindings::VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler)
{
    FN("OGLShaderResourceBindings::VSetTexture");

    OGLTexture2DPtr oglTexture = std::dynamic_pointer_cast<OGLTexture2D>(texture);
    if (oglTexture == nullptr)
    {
        LOG_ERROR("Texture bound to '%s' was not created by the OpenGL device.", name);
        return;
    }

    // A null sampler is allowed: the texture then keeps whatever filtering and
    // wrapping it was created with, which is how OpenGL behaved before sampler
    // objects existed.
    TextureBinding binding;
    binding.texture = oglTexture;
    binding.sampler = std::dynamic_pointer_cast<OGLTextureSampler>(sampler);

    m_textures[name] = binding;
}

void OGLShaderResourceBindings::VSetAccelerationStructure(const char *name, void * /*accelerationStructure*/)
{
    FN("OGLShaderResourceBindings::VSetAccelerationStructure");

    LOG_ERROR("Ray tracing is not supported in OpenGL, ignoring acceleration structure '%s'.", name);
}

void OGLShaderResourceBindings::VSetStorageImage(const char *name, Texture2DPtr texture)
{
    FN("OGLShaderResourceBindings::VSetStorageImage");

    OGLTexture2DPtr oglTexture = std::dynamic_pointer_cast<OGLTexture2D>(texture);
    if (oglTexture == nullptr)
    {
        LOG_ERROR("Storage image bound to '%s' was not created by the OpenGL device.", name);
        return;
    }

    m_storageImages[name] = oglTexture;
}

} // namespace bow
