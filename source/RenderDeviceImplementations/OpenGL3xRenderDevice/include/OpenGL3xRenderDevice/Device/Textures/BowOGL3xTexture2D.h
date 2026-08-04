#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>

#include <GL/glew.h>

namespace bow
{

class OGLTextureSampler;

class OGLTexture2D : public ITexture2D
{
  public:
    OGLTexture2D(Texture2DDescription description, GLenum textureTarget);
    ~OGLTexture2D();

    // =========================================================================
    // Inherited via ITexture2D

    void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
    void VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
    std::shared_ptr<void> VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment);
    Texture2DDescription VGetDescription();

    // =========================================================================
    // OpenGL specific

    uint32_t GetHandle();
    GLenum GetTarget();

    void Bind();
    void BindToLastTextureUnit();
    static void UnBind(GLenum textureTarget);

  private:
    // You shall not copy
    OGLTexture2D(const OGLTexture2D &obj) = delete;
    OGLTexture2D &operator=(const OGLTexture2D &obj) = delete;

    void GenerateMipmaps();
    void ApplySampler(const OGLTextureSampler &sampler);

    const GLenum m_target;
    const Texture2DDescription m_Description;
    GLenum m_lastTextureUnit;

    uint32_t m_TextureHandle;
};

typedef std::shared_ptr<OGLTexture2D> OGLTexture2DPtr;

} // namespace bow
