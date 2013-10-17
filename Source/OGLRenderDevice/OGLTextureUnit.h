#pragma once
#include "LongBow.h"

#include "ICleanable.h"
#include "ICleanableObserver.h"

#include "OGLTexture2D.h"
#include "OGLTextureSampler.h"

namespace Bow {

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