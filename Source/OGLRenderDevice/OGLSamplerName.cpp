#include "OGLSamplerName.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLSamplerName::OGLSamplerName()
	{
		FN("OGLTextureName::OGLTextureName()");
		m_value = 0;
		glGenSamplers(1, &m_value);
	}


	OGLSamplerName::~OGLSamplerName()
	{
		FN("OGLTextureName::~OGLTextureName()");
		if (m_value != 0)
        {
            glDeleteSamplers(1, &m_value);
            m_value = 0;
        }
	}


	unsigned int OGLSamplerName::GetValue()
	{
		FN("OGLTextureName::GetValue()");
		return m_value;
	}

}