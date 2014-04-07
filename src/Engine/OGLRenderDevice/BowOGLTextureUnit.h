#pragma once
#include "BowPrerequisites.h"

#include "IBowCleanableObserver.h"

namespace Bow {
	namespace Renderer {

		using namespace Core;

		typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;
		typedef std::shared_ptr<class OGLTextureSampler> OGLTextureSamplerPtr;
		typedef unsigned int GLenum;

		class OGLTextureUnit : ICleanable
		{
		public:
			OGLTextureUnit(unsigned int index, ICleanableObserver* observer);

			OGLTexture2DPtr GetTexture();
			void SetTexture(OGLTexture2DPtr texture);

			OGLTextureSamplerPtr GetSampler();
			void SetSampler(OGLTextureSamplerPtr sampler);

			void CleanLastTextureUnit();
			void Clean();
		private:
			void Validate();

			typedef enum TYPE_DirtyFlags
			{
				None = 0,
				Texture = 1,
				TextureSampler = 2,
				All = Texture | TextureSampler
			} DirtyFlags;

			const int					m_textureUnitIndex;
			const GLenum				m_textureUnit;
			ICleanableObserver* const	m_observer;

			OGLTexture2DPtr			m_texture;
			OGLTextureSamplerPtr	m_textureSampler;
			DirtyFlags				m_dirtyFlags;
		};

		typedef std::shared_ptr<OGLTextureUnit> OGLTextureUnitPtr;

	}
}