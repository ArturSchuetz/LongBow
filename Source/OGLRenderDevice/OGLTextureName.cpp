#include "OGLTextureName.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLTextureName::OGLTextureName()
	{
		FN("OGLTextureName::OGLTextureName()");
		m_value = 0;
		glGenTextures(1, &m_value);
	}


	OGLTextureName::~OGLTextureName()
	{
		FN("OGLTextureName::~OGLTextureName()");
		if (m_value != 0)
        {
            glDeleteTextures(1, &m_value);
            m_value = 0;
        }
	}


	unsigned int OGLTextureName::GetValue()
	{
		FN("OGLTextureName::GetValue()");
		return m_value;
	}

}