#pragma once
#include "LongBow.h"

#include <GL\glew.h>

namespace Bow {

	class OGLBufferName
    {
	public:
		OGLBufferName();
        ~OGLBufferName();

        unsigned int GetValue();

	private:
		// You shall not copy"
		OGLBufferName( const OGLBufferName &obj){}
		unsigned int m_value;
    };

	typedef std::shared_ptr<OGLBufferName> OGLBufferNamePtr;
}