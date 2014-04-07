#include "BowOGLSamplerName.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLSamplerName::OGLSamplerName()
		{
			m_value = 0;
			glGenSamplers(1, &m_value);
		}


		OGLSamplerName::~OGLSamplerName()
		{
			if (m_value != 0)
			{
				glDeleteSamplers(1, &m_value);
				m_value = 0;
			}
		}


		unsigned int OGLSamplerName::GetValue()
		{
			return m_value;
		}

	}
}