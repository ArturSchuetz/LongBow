#pragma once
#include "BowOGL3xFragmentOutputs.h"
#include "BowLogger.h"

#include <GL/glew.h>

namespace bow {

	OGLFragmentOutputs::OGLFragmentOutputs(unsigned int program)
	{
		m_ShaderProgramHandle = program;
	}


	OGLFragmentOutputs::~OGLFragmentOutputs()
	{
	}


	int OGLFragmentOutputs::operator[](std::string index) const
	{
		unsigned int i = -1;
		i = glGetFragDataLocation(m_ShaderProgramHandle, index.c_str());

		if (i == -1)
		{
			LOG_ASSERT(i != -1, "Fragment Output Key does not exist.");
			return -1;
		}

		return i;
	}

}
