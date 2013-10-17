#pragma once
#include "LongBow.h"

namespace Bow {

	class OGLSamplerName
	{
	public:
		OGLSamplerName();
		~OGLSamplerName();

		unsigned int GetValue();

	private:
		OGLSamplerName( const OGLSamplerName &obj){}
		unsigned int m_value;
	};

	typedef std::shared_ptr<OGLSamplerName> OGLSamplerNamePtr;

}