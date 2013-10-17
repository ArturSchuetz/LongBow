#pragma once
#include "LongBow.h"

#include "IBowTexture2D.h"
#include "OGLTextureName.h"
#include "OGLTextureSampler.h"

#include <GL\glew.h>

namespace Bow {

	class OGLTexture2D : public ITexture2D
	{
	public:
		OGLTexture2D(Texture2DDescription description, GLenum textureTarget);

		OGLTextureNamePtr GetHandle();
        GLenum GetTarget();

		void Bind();
		void BindToLastTextureUnit();
		static void OGLTexture2D::UnBind(GLenum textureTarget);

		void CopyFromBuffer( WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
        void CopyFromSystemMemory(void* bitmapInSystemMemory, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
        VoidPtr CopyToSystemMemory( ImageFormat format, ImageDatatype dataType, int rowAlignment);

        Texture2DDescription GetDescription();

	private:
		void GenerateMipmaps();
		void ApplySampler(OGLTextureSampler sampler);

		const OGLTextureNamePtr		m_name;
        const GLenum				m_target;
        const Texture2DDescription	m_description;
        GLenum						m_lastTextureUnit;
	};

	typedef std::shared_ptr<OGLTexture2D> OGLTexture2DPtr;

}