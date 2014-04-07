#pragma once
#include "BowOGLFragmentOutputs.h"
#include "BowLogger.h"
#include "BowOGLShaderProgramName.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLFragmentOutputs::OGLFragmentOutputs(OGLShaderProgramNamePtr program)
		{
			LOG_ASSERT(program.get() != nullptr, "OGLShaderProgramNamePtr is a nullptr.");

			m_program = program;
		}


		OGLFragmentOutputs::~OGLFragmentOutputs()
		{
		}


		int OGLFragmentOutputs::operator[](std::string index) const
		{
			unsigned int i = -1;
			i = glGetFragDataLocation(m_program->GetValue(), index.c_str());

			if (i == -1)
			{
				LOG_ASSERT(i != -1, "Fragment Output Key does not exist.");
				return -1;
			}

			return i;
		}

	}
}