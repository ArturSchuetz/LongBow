#include "BowOGLShaderObject.h"
#include "BowLogger.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLShaderObject::OGLShaderObject(GLenum shaderType, std::string source)
		{
			GLint length;

			/* create shader object, set the source, and compile */
			m_shaderObject = glCreateShader(shaderType);
			if (m_shaderObject == 0)
				LOG_ERROR("Could not create Shaderobject.");

			length = strlen(source.c_str());
			const GLchar* shaderSource = source.c_str();
			glShaderSource(m_shaderObject, 1, &shaderSource, &length);
			glCompileShader(m_shaderObject);


			/* make sure the compilation was successful */
			glGetShaderiv(m_shaderObject, GL_COMPILE_STATUS, &m_result);
			if (m_result != GL_FALSE)
				LOG_DEBUG("Shader successfully loaded!");
			else
				LOG_ERROR(GetCompileLog().c_str());
		}


		bool OGLShaderObject::IsReady()
		{
			return m_result != GL_FALSE;
		}


		unsigned int OGLShaderObject::GetShader()
		{
			return m_shaderObject;
		}


		OGLShaderObject::~OGLShaderObject()
		{
			glDeleteShader(m_shaderObject);
		}


		std::string OGLShaderObject::GetCompileLog()
		{
			char* buffer;
			GLint length, result;

			/* get the shader info log */
			glGetShaderiv(m_shaderObject, GL_INFO_LOG_LENGTH, &length);
			buffer = (char*)malloc(length);

			glGetShaderInfoLog(m_shaderObject, length, &result, buffer);

			return std::string(buffer);
		}

	}
}