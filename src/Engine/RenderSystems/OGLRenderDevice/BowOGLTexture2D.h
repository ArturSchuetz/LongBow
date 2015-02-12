#pragma once
#include "BowPrerequisites.h"

#include "IBowTexture2D.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		class OGLTextureSampler;

		class OGLTexture2D : public ITexture2D
		{
		public:
			OGLTexture2D(Texture2DDescription description, GLenum textureTarget);
			~OGLTexture2D();

			unsigned int GetHandle();
			GLenum GetTarget();

			void Bind();
			void BindToLastTextureUnit();
			static void OGLTexture2D::UnBind(GLenum textureTarget);

			void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
			void VCopyFromSystemMemory(void* bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
			std::shared_ptr<void> VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment);

			Texture2DDescription VGetDescription();

		private:
			void GenerateMipmaps();
			void ApplySampler(OGLTextureSampler sampler);

			const GLenum				m_target;
			const Texture2DDescription	m_Description;
			GLenum						m_lastTextureUnit;

			unsigned int				m_TextureHandle;
		};

		typedef std::shared_ptr<OGLTexture2D> OGLTexture2DPtr;

	}
}