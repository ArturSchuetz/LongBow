#include "BowOGLBufferName.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLBufferName::OGLBufferName()
		{
			m_value = 0;
			glGenBuffers(1, &m_value);
		}

		OGLBufferName::~OGLBufferName()
		{
			if (m_value != 0)
			{
				glDeleteBuffers(1, &m_value);
				m_value = 0;
			}
		}

		unsigned int OGLBufferName::GetValue()
		{
			return m_value;
		}

	}
}