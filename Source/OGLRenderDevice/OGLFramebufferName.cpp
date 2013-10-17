#include "OGLFramebufferName.h"

#include <GL\glew.h>

namespace Bow {

	OGLFramebufferName::OGLFramebufferName()
    {
		FN("OGLFramebufferName::OGLFramebufferName()");
		m_value = 0;
		glGenFramebuffers(1, &m_value);
    }

	OGLFramebufferName::~OGLFramebufferName()
    {
		FN("OGLFramebufferName::~OGLFramebufferName()");
        if (m_value != 0)
        {
			glDeleteFramebuffers(1, &m_value);
            m_value = 0;
        }
    }

	unsigned int OGLFramebufferName::GetValue()
    {
		FN("OGLFramebufferName::GetValue()");
		return m_value;
    }



}