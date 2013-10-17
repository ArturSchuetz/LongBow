#include "OGLBufferName.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLBufferName::OGLBufferName()
    {
		FN("OGLBufferName::OGLBufferName()");
		m_value = 0;
        glGenBuffers(1, &m_value);
    }

	OGLBufferName::~OGLBufferName()
    {
		FN("OGLBufferName::~OGLBufferName()");
        if (m_value != 0)
        {
            glDeleteBuffers(1, &m_value);
            m_value = 0;
        }
    }

	unsigned int OGLBufferName::GetValue()
    {
		FN("OGLBufferName::GetValue()");
		return m_value;
    }

}