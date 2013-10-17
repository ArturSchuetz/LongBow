#pragma once
#include "LongBow.h"

#include <GL\glew.h>

namespace Bow {

	class OGLFramebufferName
    {
	public:
		OGLFramebufferName();
        ~OGLFramebufferName();

        unsigned int GetValue();

	private:
		// You shall not copy"
		OGLFramebufferName( const OGLFramebufferName &obj){}
		unsigned int m_value;
    };

	typedef std::shared_ptr<OGLFramebufferName> OGLFramebufferNamePtr;
}