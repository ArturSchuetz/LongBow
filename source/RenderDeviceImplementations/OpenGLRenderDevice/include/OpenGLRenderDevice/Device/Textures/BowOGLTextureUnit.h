#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>

#include <CoreSystems/DesignPattern/IBowCleanableObserver.h>

namespace bow
{

typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;
typedef std::shared_ptr<class OGLTextureSampler> OGLTextureSamplerPtr;
typedef uint32_t GLenum;

class OGLTextureUnit : ICleanable
{
  public:
    OGLTextureUnit(uint32_t index, ICleanableObserver *observer);

    OGLTexture2DPtr GetTexture();
    void SetTexture(OGLTexture2DPtr texture);

    OGLTextureSamplerPtr GetSampler();
    void SetSampler(OGLTextureSamplerPtr sampler);

    void CleanLastTextureUnit();
    void Clean();

  private:
    // You shall not copy
    OGLTextureUnit(const OGLTextureUnit &obj) = delete;
    OGLTextureUnit &operator=(const OGLTextureUnit &obj) = delete;

    void Validate();

    typedef enum TYPE_DirtyFlags
    {
        None = 0,
        Texture = 1,
        TextureSampler = 2,
        All = Texture | TextureSampler
    } DirtyFlags;

    const int m_textureUnitIndex;
    const GLenum m_textureUnit;
    ICleanableObserver *const m_observer;

    OGLTexture2DPtr m_texture;
    OGLTextureSamplerPtr m_textureSampler;
    DirtyFlags m_DirtyFlags;
};

typedef std::shared_ptr<OGLTextureUnit> OGLTextureUnitPtr;

} // namespace bow
