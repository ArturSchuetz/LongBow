#include "BowOGLTextureName.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLTextureName::OGLTextureName()
		{
			m_value = 0;
			glGenTextures(1, &m_value);
		}


		OGLTextureName::~OGLTextureName()
		{
			if (m_value != 0)
			{
				glDeleteTextures(1, &m_value);
				m_value = 0;
			}
		}


		unsigned int OGLTextureName::GetValue()
		{
			return m_value;
		}

	}
}