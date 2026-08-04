#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/IBowShaderResourceBindings.h>

#include <string>
#include <unordered_map>

namespace bow
{

typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;
typedef std::shared_ptr<class OGLTextureSampler> OGLTextureSamplerPtr;
typedef std::shared_ptr<class OGLShaderResourceBuffer> OGLShaderResourceBufferPtr;
typedef std::shared_ptr<class OGLStorageBuffer> OGLStorageBufferPtr;

//! What OpenGL 3.x has instead of a descriptor set.
/*!
    Vulkan and DirectX 12 hand the driver a prepared set of resources; OpenGL
    binds them one at a time against the currently active program. This class
    therefore does not talk to the driver at all -- it only records what the
    caller asked for, keyed by the name used in the shader, and the render
    context applies the recorded state right before a draw.

    Binding by name rather than by slot is what keeps the interface the same
    across backends: a GLSL `layout(binding = 0) uniform sampler2D diffuseTex`
    is addressed as "diffuseTex" here just as it is under Vulkan.
*/
class OGLShaderResourceBindings : public IShaderResourceBindings
{
  public:
    struct TextureBinding
    {
        OGLTexture2DPtr texture;
        OGLTextureSamplerPtr sampler;
    };

    OGLShaderResourceBindings();
    ~OGLShaderResourceBindings();

    void VSetBuffer(const char *name, UniformBufferPtr uniformBuffer) override;
    void VSetBuffer(const char *name, StorageBufferPtr storageBuffer) override;
    void VSetTexture(const char *name, Texture2DPtr texture, TextureSamplerPtr sampler) override;
    void VSetAccelerationStructure(const char *name, void *accelerationStructure) override;
    void VSetStorageImage(const char *name, Texture2DPtr texture) override;

    const std::unordered_map<std::string, TextureBinding> &GetTextures() const { return m_textures; }
    const std::unordered_map<std::string, OGLShaderResourceBufferPtr> &GetUniformBuffers() const { return m_uniformBuffers; }
    const std::unordered_map<std::string, OGLStorageBufferPtr> &GetStorageBuffers() const { return m_storageBuffers; }
    const std::unordered_map<std::string, OGLTexture2DPtr> &GetStorageImages() const { return m_storageImages; }

  private:
    // You shall not copy
    OGLShaderResourceBindings(const OGLShaderResourceBindings &) = delete;
    OGLShaderResourceBindings &operator=(const OGLShaderResourceBindings &) = delete;

    std::unordered_map<std::string, TextureBinding> m_textures;
    std::unordered_map<std::string, OGLShaderResourceBufferPtr> m_uniformBuffers;
    std::unordered_map<std::string, OGLStorageBufferPtr> m_storageBuffers;
    std::unordered_map<std::string, OGLTexture2DPtr> m_storageImages;
};

typedef std::shared_ptr<OGLShaderResourceBindings> OGLShaderResourceBindingsPtr;

} // namespace bow
