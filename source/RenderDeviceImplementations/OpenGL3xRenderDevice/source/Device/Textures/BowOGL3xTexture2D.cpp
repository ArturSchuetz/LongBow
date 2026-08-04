#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTexture2D.h>

#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xReadPixelBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xWritePixelBuffer.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTextureSampler.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{
namespace
{

//! Number of mip levels a complete chain needs for the given size.
uint32_t MipLevelCount(int width, int height)
{
    uint32_t levels = 1;
    int size = (width > height) ? width : height;
    while (size > 1)
    {
        size >>= 1;
        ++levels;
    }
    return levels;
}

} // namespace

OGLTexture2D::OGLTexture2D(Texture2DDescription description, GLenum textureTarget) : m_Description(description), m_target(textureTarget), m_TextureHandle(0)
{
    FN("OGLTexture2D::OGLTexture2D");

    m_TextureHandle = 0;
    // glCreateTextures returns a name that already refers to a texture object
    // of the given target. glGenTextures only reserves the name, which is why
    // the old code had to bind before it could do anything with it.
    LOG_TRACE("glCreateTextures");
    glCreateTextures(m_target, 1, &m_TextureHandle);

    LOG_ASSERT(!(description.GetWidth() <= 0), "description.Width must be greater than zero.");
    LOG_ASSERT(!(description.GetHeight() <= 0), "description.Height must be greater than zero.");

    if (description.GenerateMipmaps())
    {
        LOG_ASSERT(textureTarget != GL_TEXTURE_RECTANGLE, "description.GenerateMipmaps cannot be true for texture "
                                                          "rectangles.");
        // Non-power-of-two textures have had complete mipmap chains since
        // OpenGL 2.0; the restriction this used to assert was a GL 1.x one.
    }

    int numberOfTextureUnits;
    LOG_TRACE("glGetIntegerv");
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numberOfTextureUnits);
    m_lastTextureUnit = GL_TEXTURE0 + (numberOfTextureUnits - 1);

    OGLWritePixelBuffer::UnBind();

    m_mipLevels = description.GenerateMipmaps() ? MipLevelCount(description.GetWidth(), description.GetHeight()) : 1;

    // Immutable storage: format and level count are fixed here and cannot be
    // redefined afterwards. That is the point -- the driver validates once
    // instead of on every upload, and an upload can no longer silently change
    // the texture's format, which is what the old VCopyFromSystemMemory did.
    LOG_TRACE("glTextureStorage2D");
    glTextureStorage2D(m_TextureHandle, (GLsizei)m_mipLevels, OGLTypeConverter::To(description.GetTextureFormat()), description.GetWidth(), description.GetHeight());

    //
    // Default sampler, compatiable when attaching a non-mimapped
    // texture to a frame buffer object.
    //
    ApplySampler(OGLTextureSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp, 1));
}

OGLTexture2D::~OGLTexture2D()
{
    FN("OGLTexture2D::~OGLTexture2D");

    if (m_TextureHandle != 0)
    {
        LOG_TRACE("glDeleteTextures");
        glDeleteTextures(1, &m_TextureHandle);
        m_TextureHandle = 0;
    }
}

uint32_t OGLTexture2D::GetHandle()
{
    FN("OGLTexture2D::GetHandle");

    return m_TextureHandle;
}

GLenum OGLTexture2D::GetTarget()
{
    FN("OGLTexture2D::GetTarget");

    return m_target;
}

void OGLTexture2D::Bind()
{
    FN("OGLTexture2D::Bind");

    // Without direct state access a texture had to be made current on some
    // unit before it could be touched at all; binding now only means "make
    // this visible to the shader".
    LOG_TRACE("glBindTextureUnit");
    glBindTextureUnit(0, m_TextureHandle);
}

void OGLTexture2D::BindToLastTextureUnit()
{
    FN("OGLTexture2D::BindToLastTextureUnit");

    LOG_TRACE("glBindTextureUnit");
    glBindTextureUnit(m_lastTextureUnit, m_TextureHandle);
}

void OGLTexture2D::UnBind(GLenum textureTarget)
{
    FN("OGLTexture2D::UnBind");

    LOG_TRACE("glBindTextureUnit");
    glBindTextureUnit(0, 0);
}

void OGLTexture2D::VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    FN("OGLTexture2D::VCopyFromBuffer");
    OPTICK_EVENT();

    LOG_ASSERT(!(pixelBuffer->VGetSizeInBytes() < RequiredSizeInBytes(width, height, format, dataType, rowAlignment)), "Pixel buffer is not big enough for provided width, height, format, "
                                                                                                                       "and datatype.");
    LOG_ASSERT(!(xOffset < 0), "xOffset must be greater than or equal to zero.");
    LOG_ASSERT(!(yOffset < 0), "yOffset must be greater than or equal to zero.");
    LOG_ASSERT(!((xOffset + width) > m_Description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
    LOG_ASSERT(!((yOffset + height) > m_Description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

    VerifyRowAlignment(rowAlignment);

    OGLWritePixelBufferPtr bufferObjectGL = std::dynamic_pointer_cast<OGLWritePixelBuffer>(pixelBuffer);

    bufferObjectGL->Bind();
    BindToLastTextureUnit();
    LOG_TRACE("glPixelStorei");
    glPixelStorei(GL_UNPACK_ALIGNMENT, rowAlignment);
    LOG_TRACE("glTextureSubImage2D");
    glTextureSubImage2D(m_TextureHandle, 0, xOffset, yOffset, width, height, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), nullptr);

    GenerateMipmaps();
}

void OGLTexture2D::VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    FN("OGLTexture2D::VCopyFromSystemMemory");
    OPTICK_EVENT();

    LOG_ASSERT(!((width) > m_Description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
    LOG_ASSERT(!((height) > m_Description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

    VerifyRowAlignment(rowAlignment);

    ApplySampler(OGLTextureSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp));

    LOG_TRACE("glPixelStorei");
    glPixelStorei(GL_UNPACK_ALIGNMENT, rowAlignment);

    // This used to call glTexImage2D with a hardcoded GL_RGBA internal format,
    // reallocating the texture and discarding the format it was created with.
    LOG_TRACE("glTextureSubImage2D");
    glTextureSubImage2D(m_TextureHandle, 0, 0, 0, width, height, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), bitmapInSystemMemory);

    GenerateMipmaps();
}

std::shared_ptr<void> OGLTexture2D::VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    FN("OGLTexture2D::VCopyToSystemMemory");
    OPTICK_EVENT();

    LOG_ASSERT(!(format == ImageFormat::StencilIndex), "StencilIndex is not supported by CopyToBuffer, .Try "
                                                       "DepthStencil instead");

    VerifyRowAlignment(rowAlignment);

    BindToLastTextureUnit();

    LOG_TRACE("glPixelStorei");
    glPixelStorei(GL_PACK_ALIGNMENT, rowAlignment);

    GLint textureformat;
    LOG_TRACE("glGetTexLevelParameteriv");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &textureformat);

    GLint width;
    LOG_TRACE("glGetTexLevelParameteriv");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);

    GLint height;
    LOG_TRACE("glGetTexLevelParameteriv");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    unsigned char *pixels = new unsigned char[(int)width * (int)height * NumberOfChannels(format) * SizeInBytes(dataType)];

    LOG_TRACE("glGetTexImage");
    glGetTexImage(m_target, 0, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), pixels);

    // glReadPixels(0, 0, width, height, OGLTypeConverter::To(format),
    // OGLTypeConverter::To(dataType), pixels);

    return std::shared_ptr<unsigned char>(pixels, [](unsigned char *p) { delete[] p; });
}

Texture2DDescription OGLTexture2D::VGetDescription()
{
    FN("OGLTexture2D::VGetDescription");

    return m_Description;
}

void OGLTexture2D::GenerateMipmaps()
{
    FN("OGLTexture2D::GenerateMipmaps");

    if (m_Description.GenerateMipmaps())
    {
        LOG_TRACE("glGenerateTextureMipmap");
        glGenerateTextureMipmap(m_TextureHandle);
    }
}

void OGLTexture2D::ApplySampler(const OGLTextureSampler &sampler)
{
    FN("OGLTexture2D::ApplySampler");

    // The minification filter used to be taken from MagnificationFilter too,
    // so a texture's minification setting was silently ignored.
    GLenum minFilter = OGLTypeConverter::To(sampler.MinificationFilter);
    GLenum magFilter = OGLTypeConverter::To(sampler.MagnificationFilter);
    GLenum wrapS = OGLTypeConverter::To(sampler.WrapS);
    GLenum wrapT = OGLTypeConverter::To(sampler.WrapT);

    LOG_TRACE("glTextureParameteri");
    glTextureParameteri(m_TextureHandle, GL_TEXTURE_MIN_FILTER, (int)minFilter);
    glTextureParameteri(m_TextureHandle, GL_TEXTURE_MAG_FILTER, (int)magFilter);
    glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_S, (int)wrapS);
    glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_T, (int)wrapT);
}

} // namespace bow
