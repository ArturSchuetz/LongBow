#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

namespace bow
{

class OGLTextureSampler : public ITextureSampler
{
  public:
    OGLTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1);
    ~OGLTextureSampler();

    void Bind(int textureUnitIndex);
    static void UnBind(int textureUnitIndex);

  private:
    // You shall not copy
    OGLTextureSampler(const OGLTextureSampler &obj) = delete;
    OGLTextureSampler &operator=(const OGLTextureSampler &obj) = delete;

    uint32_t m_SamplerHandle;
};

typedef std::shared_ptr<OGLTextureSampler> OGLTextureSamplerPtr;

} // namespace bow
