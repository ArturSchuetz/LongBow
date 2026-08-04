#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>

#include <CoreSystems/DesignPattern/IBowCleanableObserver.h>

namespace bow
{

typedef std::shared_ptr<class OGLTextureUnit> OGLTextureUnitPtr;
typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;
typedef std::shared_ptr<class OGLTextureSampler> OGLTextureSamplerPtr;

class OGLTextureUnits : ICleanableObserver
{
  public:
    OGLTextureUnits();

    void Clean();
    void NotifyDirty(ICleanable *value);

    int GetMaxTextureUnits();
    void SetTexture(int index, OGLTexture2DPtr texture);
    void SetSampler(int index, OGLTextureSamplerPtr sampler);

  private:
    // You shall not copy
    OGLTextureUnits(const OGLTextureUnits &obj) = delete;
    OGLTextureUnits &operator=(const OGLTextureUnits &obj) = delete;

    std::unordered_map<uint32_t, OGLTextureUnitPtr> m_textureUnits;
    std::list<ICleanable *> m_dirtyTextureUnits;
    OGLTextureUnitPtr m_lastTextureUnit;
    int m_numberOfTextureUnits;
};

typedef std::shared_ptr<OGLTextureUnits> OGLTextureUnitsPtr;

} // namespace bow
