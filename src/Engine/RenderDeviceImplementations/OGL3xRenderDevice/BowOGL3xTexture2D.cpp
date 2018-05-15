#include "BowOGL3xTexture2D.h"
#include "BowLogger.h"

#include "BowOGL3xTextureSampler.h"

#include "BowOGL3xTypeConverter.h"
#include "BowOGL3xReadPixelBuffer.h"
#include "BowOGL3xWritePixelBuffer.h"

#include <GL/glew.h>

namespace bow {

	OGLTexture2D::OGLTexture2D(Texture2DDescription description, GLenum textureTarget) : m_Description(description), m_target(textureTarget), m_TextureHandle(0)
	{
		m_TextureHandle = 0;
		glGenTextures(1, &m_TextureHandle);

		LOG_ASSERT(!(description.GetWidth() <= 0), "description.Width must be greater than zero.");
		LOG_ASSERT(!(description.GetHeight() <= 0), "description.Height must be greater than zero.");

		if (description.GenerateMipmaps())
		{
			LOG_ASSERT(textureTarget != GL_TEXTURE_RECTANGLE, "description.GenerateMipmaps cannot be true for texture rectangles.");
			LOG_ASSERT(IsPowerOfTwo(description.GetWidth()), "When description.GenerateMipmaps is true, the width must be a power of two.");
			LOG_ASSERT(IsPowerOfTwo(description.GetHeight()), "When description.GenerateMipmaps is true, the height must be a power of two.");
		}

		int numberOfTextureUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numberOfTextureUnits);
		m_lastTextureUnit = GL_TEXTURE0 + (numberOfTextureUnits - 1);

		OGLWritePixelBuffer::UnBind();
		BindToLastTextureUnit();

		glTexImage2D( m_target, 0,
			OGLTypeConverter::To( description.GetTextureFormat() ),
			description.GetWidth(),
			description.GetHeight(),
			0,
			OGLTypeConverter::TextureToPixelFormat( description.GetTextureFormat() ),
			OGLTypeConverter::TextureToPixelType( description.GetTextureFormat() ),
			nullptr );

		//
		// Default sampler, compatiable when attaching a non-mimapped 
		// texture to a frame buffer object.
		//
		ApplySampler(OGLTextureSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp, 1));
	}


	OGLTexture2D::~OGLTexture2D()
	{
		if (m_TextureHandle != 0)
		{
			glDeleteTextures(1, &m_TextureHandle);
			m_TextureHandle = 0;
		}
	}


	unsigned int OGLTexture2D::GetHandle()
	{
		return m_TextureHandle;
	}


	GLenum OGLTexture2D::GetTarget()
	{
		return m_target;
	}


	void OGLTexture2D::Bind()
	{
		glBindTexture(m_target, m_TextureHandle);
	}


	void OGLTexture2D::BindToLastTextureUnit()
	{
		glActiveTexture(m_lastTextureUnit);
		Bind();
	}


	void OGLTexture2D::UnBind(GLenum textureTarget)
	{
		glBindTexture(textureTarget, 0);
	}


	void OGLTexture2D::VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		LOG_ASSERT(!(pixelBuffer->VGetSizeInBytes() < RequiredSizeInBytes(width, height, format, dataType, rowAlignment)), "Pixel buffer is not big enough for provided width, height, format, and datatype.");
		LOG_ASSERT(!(xOffset < 0), "xOffset must be greater than or equal to zero.");
		LOG_ASSERT(!(yOffset < 0), "yOffset must be greater than or equal to zero.");
		LOG_ASSERT(!((xOffset + width) > m_Description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
		LOG_ASSERT(!((yOffset + height) > m_Description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

		VerifyRowAlignment(rowAlignment);

		OGLWritePixelBufferPtr bufferObjectGL = std::dynamic_pointer_cast<OGLWritePixelBuffer>(pixelBuffer);

		bufferObjectGL->Bind();
		BindToLastTextureUnit();
		glPixelStorei(GL_UNPACK_ALIGNMENT, rowAlignment);
		glTexSubImage2D(m_target, 0, xOffset, yOffset, width, height, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), nullptr);

		GenerateMipmaps();
	}


	void OGLTexture2D::VCopyFromSystemMemory(void* bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		LOG_ASSERT(!((width) > m_Description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
		LOG_ASSERT(!((height) > m_Description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

		VerifyRowAlignment(rowAlignment);

		BindToLastTextureUnit();

		ApplySampler(OGLTextureSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp));
		glTexImage2D(m_target, 0, GL_RGBA, width, height, 0, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), bitmapInSystemMemory);

		GenerateMipmaps();
	}


	std::shared_ptr<void> OGLTexture2D::VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		LOG_ASSERT(!(format == ImageFormat::StencilIndex), "StencilIndex is not supported by CopyToBuffer, .Try DepthStencil instead");

		VerifyRowAlignment(rowAlignment);

		BindToLastTextureUnit();

		glPixelStorei(GL_PACK_ALIGNMENT, rowAlignment);

		GLint textureformat;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &textureformat);

		GLint width;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);

		GLint height;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

		unsigned char* pixels = new unsigned char[(int)width * (int)height * 3];
			
		glGetTexImage(m_target, 0, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), pixels);

		//glReadPixels(0, 0, width, height, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), pixels);

		return std::shared_ptr<unsigned char>(pixels);
	}


	Texture2DDescription OGLTexture2D::VGetDescription()
	{
		return m_Description;
	}


	void OGLTexture2D::GenerateMipmaps()
	{
		if (m_Description.GenerateMipmaps())
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}


	void OGLTexture2D::ApplySampler(OGLTextureSampler sampler)
	{
		GLenum minFilter = OGLTypeConverter::To(sampler.MagnificationFilter);
		GLenum magFilter = OGLTypeConverter::To(sampler.MagnificationFilter);
		GLenum wrapS = OGLTypeConverter::To(sampler.WrapS);
		GLenum wrapT = OGLTypeConverter::To(sampler.WrapT);

		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, (int)minFilter);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, (int)magFilter);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, (int)wrapS);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, (int)wrapT);
	}

}
