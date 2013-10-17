#include "OGLTextureUnits.h"

#include <GL\glew.h>

namespace Bow {

	OGLTextureUnits::OGLTextureUnits() : m_numberOfTextureUnits(0)
	{
		FN("OGLTextureUnits::OGLTextureUnits()");
        // Device.NumberOfTextureUnits is not initialized yet.
        //		
		LOG(LOG_COLOR_INFO, "glGetIntegerv();");
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_numberOfTextureUnits);
		m_numberOfTextureUnits = min(m_numberOfTextureUnits, 31);

        for (int i = 0; i < m_numberOfTextureUnits; ++i)
        {
            m_textureUnits[i] = OGLTextureUnitPtr( new OGLTextureUnit(i, this) );
        }
        m_dirtyTextureUnits = std::list<ICleanable*>();
        m_lastTextureUnit = m_textureUnits[m_numberOfTextureUnits - 1];
	}

	void OGLTextureUnits::Clean()
	{
		FN("OGLTextureUnits::Clean()");
		for (auto it = m_dirtyTextureUnits.begin(); it != m_dirtyTextureUnits.end(); it++)
        {
            (*it)->Clean();
        }
		m_dirtyTextureUnits.clear();
		m_lastTextureUnit->CleanLastTextureUnit();
	}

	void OGLTextureUnits::NotifyDirty(ICleanable* value)
	{
		FN("OGLTextureUnits::NotifyDirty()");
		m_dirtyTextureUnits.push_back(value);
	}

	int OGLTextureUnits::GetMaxTextureUnits()
	{
		FN("OGLTextureUnits::GetMaxTextureUnits()");
		return m_numberOfTextureUnits;
	}

	void OGLTextureUnits::SetTexture(int index, OGLTexture2DPtr texture)
	{
		FN("OGLTextureUnits::SetTexture()");
		m_textureUnits[index]->SetTexture(texture);
	}

	void OGLTextureUnits::SetSampler(int index, OGLTextureSamplerPtr sampler)
	{
		FN("OGLTextureUnits::SetSampler()");
		m_textureUnits[index]->SetSampler(sampler);
	}

}