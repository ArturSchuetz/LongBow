#pragma once
#include "OGLTextureUnit.h"

namespace Bow {

	class OGLTextureUnits : ICleanableObserver
	{
	public:
		OGLTextureUnits();

		void Clean();
		void NotifyDirty(ICleanable* value);
		
		int		GetMaxTextureUnits();
		void	SetTexture(int index, OGLTexture2DPtr texture);
		void	SetSampler(int index, OGLTextureSamplerPtr sampler);

	private:
		std::hash_map<int, OGLTextureUnitPtr>	m_textureUnits;
        std::list<ICleanable*>					m_dirtyTextureUnits;
        OGLTextureUnitPtr						m_lastTextureUnit;
		int										m_numberOfTextureUnits;
	};

	typedef std::shared_ptr<OGLTextureUnits> OGLTextureUnitsPtr;

}