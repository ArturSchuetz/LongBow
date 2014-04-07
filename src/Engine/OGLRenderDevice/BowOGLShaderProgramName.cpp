#include "BowOGLShaderProgramName.h"
#include "BowLogger.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLShaderProgramName::OGLShaderProgramName()
		{
			m_program = glCreateProgram();
			if (m_program == 0)
				LOG_ERROR("Could not create Shaderobject.");
		}

		OGLShaderProgramName::~OGLShaderProgramName()
		{
			glDeleteProgram(m_program);
		}


		unsigned int OGLShaderProgramName::GetValue()
		{
			return m_program;
		}


		std::string OGLShaderProgramName::GetInfoLog()
		{
			char* buffer;
			GLint length, result;

			/* get the shader info log */
			glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
			buffer = (char*)malloc(length);

			glGetProgramInfoLog(m_program, length, &result, buffer);

			return std::string(buffer);
		}

	}
}