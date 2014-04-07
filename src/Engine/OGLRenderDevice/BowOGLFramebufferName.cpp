#include "BowOGLFramebufferName.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLFramebufferName::OGLFramebufferName()
		{
			m_value = 0;
			glGenFramebuffers(1, &m_value);
		}

		OGLFramebufferName::~OGLFramebufferName()
		{
			if (m_value != 0)
			{
				glDeleteFramebuffers(1, &m_value);
				m_value = 0;
			}
		}

		unsigned int OGLFramebufferName::GetValue()
		{
			return m_value;
		}

	}
}