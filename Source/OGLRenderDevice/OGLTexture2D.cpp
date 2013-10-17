#include "OGLTexture2D.h"

#include "OGLTypeConverter.h"
#include "OGLReadPixelBuffer.h"
#include "OGLWritePixelBuffer.h"

#include <GL/glew.h>

namespace Bow {

	inline int NumberOfChannels(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::StencilIndex:
                return 1;
            case ImageFormat::DepthComponent:
                return 1;
            case ImageFormat::Red:
                return 1;
            case ImageFormat::Green:
                return 1;
            case ImageFormat::Blue:
                return 1;
            case ImageFormat::RedGreenBlue:
                return 3;
            case ImageFormat::RedGreenBlueAlpha:
                return 4;
            case ImageFormat::BlueGreenRed:
                return 3;
            case ImageFormat::BlueGreenRedAlpha:
                return 4;
            case ImageFormat::RedGreen:
                return 2;
            case ImageFormat::RedGreenInteger:
                return 2;
            case ImageFormat::DepthStencil:
                return 2;
            case ImageFormat::RedInteger:
                return 1;
            case ImageFormat::GreenInteger:
                return 1;
            case ImageFormat::BlueInteger:
                return 1;
            case ImageFormat::RedGreenBlueInteger:
                return 3;
            case ImageFormat::RedGreenBlueAlphaInteger:
                return 4;
            case ImageFormat::BlueGreenRedInteger:
                return 3;
            case ImageFormat::BlueGreenRedAlphaInteger:
                return 4;
        }
		LOG_ASSERT(false, "format");
		return -1;
    }

    inline int SizeInBytes(ImageDatatype dataType)
    {
        switch (dataType)
        {
            case ImageDatatype::Byte:
                return 1;
            case ImageDatatype::UnsignedByte:
                return 1;
            case ImageDatatype::Short:
                return 2;
            case ImageDatatype::UnsignedShort:
                return 2;
            case ImageDatatype::Int:
                return 4;
            case ImageDatatype::UnsignedInt:
                return 4;
            case ImageDatatype::Float:
                return 4;
            case ImageDatatype::HalfFloat:
                return 2;
            case ImageDatatype::UnsignedByte332:
                return 1;
            case ImageDatatype::UnsignedShort4444:
                return 2;
            case ImageDatatype::UnsignedShort5551:
                return 2;
            case ImageDatatype::UnsignedInt8888:
                return 4;
            case ImageDatatype::UnsignedInt1010102:
                return 4;
            case ImageDatatype::UnsignedByte233Reversed:
                return 1;
            case ImageDatatype::UnsignedShort565:
                return 2;
            case ImageDatatype::UnsignedShort565Reversed:
                return 2;
            case ImageDatatype::UnsignedShort4444Reversed:
                return 2;
            case ImageDatatype::UnsignedShort1555Reversed:
                return 2;
            case ImageDatatype::UnsignedInt8888Reversed:
                return 4;
            case ImageDatatype::UnsignedInt2101010Reversed:
                return 4;
            case ImageDatatype::UnsignedInt248:
                return 4;
            case ImageDatatype::UnsignedInt10F11F11FReversed:
                return 4;
            case ImageDatatype::UnsignedInt5999Reversed:
                return 4;
            case ImageDatatype::Float32UnsignedInt248Reversed:
                return 4;
        }

		LOG_ASSERT(false, "dataType");
		return -1;
    }

	inline void VerifyRowAlignment(int rowAlignment)
    {
        if ((rowAlignment != 1) && 
            (rowAlignment != 2) && 
            (rowAlignment != 4) && 
            (rowAlignment != 8))
        {
			LOG_ASSERT(false, "rowAlignment");
        }
    }

	inline bool IsPowerOfTwo(unsigned int i)
    {
        return (i != 0) && ((i & (i - 1)) == 0);
    }

	inline int RequiredSizeInBytes( int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
    {
        int rowSize = width * NumberOfChannels(format) * SizeInBytes(dataType);

        int remainder = (rowSize % rowAlignment);
        rowSize += (rowAlignment - remainder) % rowAlignment;

        return rowSize * height;
    }

	OGLTexture2D::OGLTexture2D(Texture2DDescription description, GLenum textureTarget) : m_description(description), m_target(textureTarget), m_name(new OGLTextureName())
	{
		FN("OGLTexture2D::OGLTexture2D()");
		LOG_ASSERT(!(description.GetWidth() <= 0), "description.Width must be greater than zero.");
        LOG_ASSERT(!(description.GetHeight() <= 0), "description.Height must be greater than zero.");

        if (description.GenerateMipmaps())
        {
			LOG_ASSERT(!(textureTarget == GL_TEXTURE_RECTANGLE), "description.GenerateMipmaps cannot be true for texture rectangles.");
			LOG_ASSERT(!IsPowerOfTwo(description.GetWidth()), "When description.GenerateMipmaps is true, the width must be a power of two.");
            LOG_ASSERT(!IsPowerOfTwo(description.GetHeight()), "When description.GenerateMipmaps is true, the height must be a power of two.");
        }

		int numberOfTextureUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numberOfTextureUnits);
		m_lastTextureUnit = GL_TEXTURE0 + (numberOfTextureUnits - 1);

		OGLWritePixelBuffer::UnBind();
        BindToLastTextureUnit();

        glTexImage2D(m_target, 0,
			OGLTypeConverter::To(description.GetTextureFormat()),
			description.GetWidth(),
            description.GetHeight(),
            0,
            OGLTypeConverter::TextureToPixelFormat(description.GetTextureFormat()),   
            OGLTypeConverter::TextureToPixelType(description.GetTextureFormat()), 
			nullptr);

        //
        // Default sampler, compatiable when attaching a non-mimapped 
        // texture to a frame buffer object.
        //
		ApplySampler(OGLTextureSampler(Bow::TextureMinificationFilter::Linear, Bow::TextureMagnificationFilter::Linear, Bow::TextureWrap::Clamp, Bow::TextureWrap::Clamp, 1));
	}

	OGLTextureNamePtr OGLTexture2D::GetHandle()
    {
		FN("OGLTexture2D::GetHandle()");
		return m_name;
    }

    GLenum OGLTexture2D::GetTarget()
    {
		FN("OGLTexture2D::GetTarget()");
		return m_target;
    }

	void OGLTexture2D::Bind()
    {
		FN("OGLTexture2D::Bind()");

		glBindTexture(m_target, m_name->GetValue());
    }

	void OGLTexture2D::BindToLastTextureUnit()
	{
		FN("OGLTexture2D::BindToLastTextureUnit()");

		glActiveTexture(m_lastTextureUnit);
        Bind();
	}

	void OGLTexture2D::UnBind(GLenum textureTarget)
    {
		FN("OGLTexture2D::UnBind()");
        glBindTexture(textureTarget, 0);
    }

	void OGLTexture2D::CopyFromBuffer( WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		FN("OGLTexture2D::CopyFromBuffer()");
		LOG_ASSERT(!(pixelBuffer->GetSizeInBytes() < RequiredSizeInBytes( width, height, format, dataType, rowAlignment)), "Pixel buffer is not big enough for provided width, height, format, and datatype.");
		LOG_ASSERT(!(xOffset < 0), "xOffset must be greater than or equal to zero.");
        LOG_ASSERT(!(yOffset < 0), "yOffset must be greater than or equal to zero.");
        LOG_ASSERT(!((xOffset + width) > m_description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
        LOG_ASSERT(!((yOffset + height) > m_description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");
            
        VerifyRowAlignment(rowAlignment);

		OGLWritePixelBufferPtr bufferObjectGL = std::dynamic_pointer_cast<OGLWritePixelBuffer>(pixelBuffer);

        bufferObjectGL->Bind();
        BindToLastTextureUnit();
		glPixelStorei(GL_UNPACK_ALIGNMENT, rowAlignment);
		glTexSubImage2D(m_target, 0, xOffset, yOffset, width, height, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), nullptr);

        GenerateMipmaps();
	}

    void OGLTexture2D::CopyFromSystemMemory(void* bitmapInSystemMemory, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		FN("OGLTexture2D::CopyFromBuffer()");
		LOG_ASSERT(!(xOffset < 0), "xOffset must be greater than or equal to zero.");
        LOG_ASSERT(!(yOffset < 0), "yOffset must be greater than or equal to zero.");
        LOG_ASSERT(!((xOffset + width) > m_description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
        LOG_ASSERT(!((yOffset + height) > m_description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");
            
        VerifyRowAlignment(rowAlignment);

        BindToLastTextureUnit();

		ApplySampler(OGLTextureSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp));
		glTexImage2D(m_target, 0, GL_RGBA, width, height, 0, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), bitmapInSystemMemory);

        GenerateMipmaps();
	}
        
    VoidPtr OGLTexture2D::CopyToSystemMemory( ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		FN("OGLTexture2D::CopyToBuffer()");
		LOG_ASSERT(!(format == ImageFormat::StencilIndex), "StencilIndex is not supported by CopyToBuffer, .Try DepthStencil instead");

        VerifyRowAlignment(rowAlignment);

        BindToLastTextureUnit();
		
		glPixelStorei(GL_PACK_ALIGNMENT, rowAlignment);
        glGetTexImage(m_target, 0, OGLTypeConverter::To(format), OGLTypeConverter::To(dataType), nullptr);

		return VoidPtr(nullptr);
	}

    Texture2DDescription OGLTexture2D::GetDescription()
	{
		FN("OGLTexture2D::GetDescription()");
		return m_description;
	}

	void OGLTexture2D::GenerateMipmaps()
    {
		FN("OGLTexture2D::GenerateMipmaps()");
        if (m_description.GenerateMipmaps())
        {
			glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    void OGLTexture2D::ApplySampler(OGLTextureSampler sampler)
    {
		FN("OGLTexture2D::ApplySampler()");
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