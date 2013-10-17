#pragma once
#include "LongBow.h"
#include "OGLShaderObject.h"

#include <GL/glew.h>

namespace Bow {


	OGLShaderObject::OGLShaderObject(GLenum shaderType, std::string source)
	{
		FN("OGLShaderObject::OGLShaderObject()");
		GLint length;

		/* create shader object, set the source, and compile */
		m_shaderObject = glCreateShader(shaderType);
		if(m_shaderObject == 0)
			LOG_ERROR("Could not create Shaderobject.");

		length = strlen(source.c_str());
		const GLchar* shaderSource = source.c_str();
		glShaderSource(m_shaderObject, 1, &shaderSource, &length);
		glCompileShader(m_shaderObject);
	

		/* make sure the compilation was successful */
		glGetShaderiv(m_shaderObject, GL_COMPILE_STATUS, &m_result);
		if(m_result != GL_FALSE)
			LOG_INIT("Shader successfully loaded!");
		else
			LOG_ERROR(GetCompileLog().c_str());
	}


	bool OGLShaderObject::IsReady()
	{
		FN("OGLShaderObject::IsReady()");
		return m_result != GL_FALSE;
	}


	unsigned int OGLShaderObject::GetShader()
	{
		FN("OGLShaderObject::GetShader()");
		return m_shaderObject;
	}


	OGLShaderObject::~OGLShaderObject()
	{
		FN("OGLShaderObject::~OGLShaderObject()");
		glDeleteShader(m_shaderObject);
	}


	std::string OGLShaderObject::GetCompileLog()
	{
		FN("OGLShaderObject::GetCompileLog()");
		char* buffer;
		GLint length, result;

		/* get the shader info log */
		glGetShaderiv(m_shaderObject, GL_INFO_LOG_LENGTH, &length);
		buffer = (char*)malloc(length);

		glGetShaderInfoLog(m_shaderObject, length, &result, buffer);

		return std::string(buffer);
	}


}