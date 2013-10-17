#include "OGLVertexArrayName.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLVertexArrayName::OGLVertexArrayName()
    {
		FN("OGLVertexArrayName::OGLVertexArrayName()");
		m_value = 0;
        glGenVertexArrays(1, &m_value);
    }

	OGLVertexArrayName::~OGLVertexArrayName()
	{
		FN("OGLVertexArrayName::~OGLVertexArrayName()");
        if (m_value != 0)
        {
            glDeleteVertexArrays(1, &m_value);
            m_value = 0;
        }
    }

	unsigned int OGLVertexArrayName::GetValue()
    {
		return m_value;
    }

}