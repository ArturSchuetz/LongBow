#pragma once
#include "BowPrerequisites.h"

#include "IBowTexture2D.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		class OGLTextureSampler;
		typedef std::shared_ptr<class OGLTextureName> OGLTextureNamePtr;

		class OGLTexture2D : public ITexture2D
		{
		public:
			OGLTexture2D(Texture2DDescription description, GLenum textureTarget);

			OGLTextureNamePtr GetHandle();
			GLenum GetTarget();

			void Bind();
			void BindToLastTextureUnit();
			static void OGLTexture2D::UnBind(GLenum textureTarget);

			void CopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
			void CopyFromSystemMemory(void* bitmapInSystemMemory, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
			std::shared_ptr<void> CopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment);

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
}