#include "OGLShaderProgram.h"

#include "OGLRenderContext.h"
#include "OGLTypeConverter.h"

#include <GL/glew.h>

namespace Bow {


	OGLShaderProgram::OGLShaderProgram(const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& fragmentShaderSource)
	{
		FN("OGLShaderProgram::OGLShaderProgram()");
		
		m_ready = false;
		m_vertexShader = OGLShaderObjectPtr(new OGLShaderObject(GL_VERTEX_SHADER, vertexShaderSource));
		if (geometryShaderSource.length() > 0)
		{
			m_geometryShader = OGLShaderObjectPtr(new OGLShaderObject(GL_GEOMETRY_SHADER, geometryShaderSource));
		}
		m_fragmentShader = OGLShaderObjectPtr(new OGLShaderObject(GL_FRAGMENT_SHADER, fragmentShaderSource));

		m_program = OGLShaderProgramNamePtr(new OGLShaderProgramName());

		int programHandle = m_program->GetValue();

		glAttachShader(programHandle, m_vertexShader->GetShader());
		if (geometryShaderSource.length() > 0)
		{
			glAttachShader(programHandle, m_geometryShader->GetShader());
		}
		glAttachShader(programHandle, m_fragmentShader->GetShader());

		glLinkProgram(programHandle);

		int linkStatus;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == 0)
		{
			LOG_ERROR(m_program->GetInfoLog().c_str());
			m_ready = false;
		}
		else
		{
			LOG_INIT("Shader successfully  linked!");
			m_ready = true;
		}
		
		m_fragmentOutputs = OGLFragmentOutputsPtr(new OGLFragmentOutputs(m_program));
		m_shaderVertexAttributes = FindVertexAttributes(m_program);

		m_uniforms = FindUniforms(m_program);
	}


	OGLShaderProgram::~OGLShaderProgram()
	{
		FN("OGLShaderProgram::~OGLShaderProgram()");
	}


	std::string	OGLShaderProgram::GetLog()
	{ 
		FN("OGLShaderProgram::GetLog()"); 
		return GetProgramInfoLog(); 
	}


	ShaderVertexAttributePtr OGLShaderProgram::GetVertexAttribute(std::string name)
	{
		FN("OGLShaderProgram::GetVertexAttributes()");
		return m_shaderVertexAttributes[name];
	}
	
	
	int OGLShaderProgram::GetFragmentOutputLocation(std::string name)
	{
		FN("OGLShaderProgram::GetFragmentOutputLocation()");
		return (*m_fragmentOutputs)[name];
	}


	UniformPtr OGLShaderProgram::GetUniform(std::string name)
	{
		FN("OGLShaderProgram::GetUniform()");
		return m_uniforms[name];
	}


	OGLShaderProgramNamePtr	OGLShaderProgram::GetProgram()		
	{
		FN("OGLShaderProgram::GetProgram()");
		LOG_ASSERT(m_program.get() != nullptr, "OGLShaderProgramNamePtr is a nullptr");
		return m_program; 
	}


	void OGLShaderProgram::Bind()
	{
		FN("OGLShaderProgram::Bind()");
		glUseProgram(m_program->GetValue());
	}


	void OGLShaderProgram::Clean()
    {
		FN("OGLShaderProgram::Clean()");
		for (auto it = m_dirtyUniforms.begin(); it != m_dirtyUniforms.end(); it++)
        {
			(*it)->Clean();
        }
		m_dirtyUniforms.clear();
    }


	bool OGLShaderProgram::IsReady()			
	{
		FN("OGLShaderProgram::IsReady()");
		return m_ready; 
	}


	void OGLShaderProgram::NotifyDirty(ICleanable* value)
    {
		FN("OGLShaderProgram::NotifyDirty()");
		m_dirtyUniforms.push_back(value);
    }
	
	// =====================================================================
	// PRIVATE FUNCTIONS
	// =====================================================================

	std::string	OGLShaderProgram::GetProgramInfoLog()
	{
		FN("OGLShaderProgram::GetProgramInfoLog()");
		LOG_ASSERT(m_program.get() != nullptr, "OGLShaderProgramNamePtr is a nullptr.");

		return m_program->GetInfoLog();
	}


	ShaderVertexAttributeMap OGLShaderProgram::FindVertexAttributes(OGLShaderProgramNamePtr Program)
	{
		FN("OGLShaderProgram::FindVertexAttributes()");
		int programHandle = Program->GetValue();

		int numberOfAttributes;
		glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &numberOfAttributes);

		int attributeNameMaxLength;
		glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeNameMaxLength);

		ShaderVertexAttributeMap vertexAttributes = ShaderVertexAttributeMap();

		if(numberOfAttributes > 0)
			LOG("VertexAttributes:");

		for (unsigned int i = 0; i < (unsigned int)numberOfAttributes; ++i)
		{
			int attributeNameLength;
			int attributeLength;
			GLenum  attributeType;
			GLchar* attributeName = new GLchar[attributeNameMaxLength];

			glGetActiveAttrib(programHandle, i, attributeNameMaxLength, &attributeNameLength, &attributeLength, &attributeType, attributeName);
			
			if (strncmp(attributeName, "gl_", strlen("gl_")) == 0)
			{
				//
				// Names starting with the reserved prefix of "gl_" have a location of -1.
				continue;
			}
			int attributeLocation = glGetAttribLocation(programHandle, attributeName);

			LOG("\tName: %s, \tLocation: %d", attributeName, attributeLocation);
			vertexAttributes.insert(std::pair<std::string, ShaderVertexAttributePtr>(attributeName, ShaderVertexAttributePtr(new ShaderVertexAttribute(attributeLocation, OGLTypeConverter::ToActiveAttribType(attributeType), attributeLength))));
		}
		return vertexAttributes;
	}


	UniformMap OGLShaderProgram::FindUniforms(OGLShaderProgramNamePtr program)
    {
		FN("OGLShaderProgram::FindUniforms()");
        int programHandle = program->GetValue();

        int numberOfUniforms;
		glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

        int uniformNameMaxLength;
		glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameMaxLength);

        UniformMap uniforms;
		if(numberOfUniforms > 0)
			LOG("Uniforms:");

        for (unsigned int i = 0; i < (unsigned int)numberOfUniforms; ++i)
        {
            int uniformNameLength;
            int uniformSize;
            GLenum uniformType;
            GLchar* uniformName = new GLchar[uniformNameMaxLength];

            glGetActiveUniform(programHandle, i, uniformNameMaxLength, &uniformNameLength, &uniformSize, &uniformType, uniformName);

			if (strncmp(uniformName, "gl_", strlen("gl_")) == 0)
			{
                //
                // Names starting with the reserved prefix of "gl_" have a location of -1.
                //
                continue;
            }

            //
            // Skip uniforms in a named block
            //
            int uniformBlockIndex;
			glGetActiveUniformsiv(programHandle, 1, &i, GL_UNIFORM_BLOCK_INDEX, &uniformBlockIndex);

            if (uniformBlockIndex != -1)
            {
                continue;
            }

            int uniformLocation = glGetUniformLocation(programHandle, uniformName);
			
			LOG("\tName: %s, \tLocation: %d, \tArraySize: %d", uniformName, uniformLocation, uniformSize);
			uniforms.insert(std::pair<std::string, UniformPtr>(uniformName, UniformPtr(new OGLUniform(uniformLocation, uniformSize, OGLTypeConverter::ToActiveUniformType(uniformType), this))));
        }
        return uniforms;
    }


	static bool IsMatrix(UniformType type)
    {
		FN("IsMatrix()");
        return
            (type == UniformType::FloatMatrix22) ||
            (type == UniformType::FloatMatrix33) ||
            (type == UniformType::FloatMatrix44) ||
            (type == UniformType::FloatMatrix23) ||
            (type == UniformType::FloatMatrix24) ||
            (type == UniformType::FloatMatrix32) ||
            (type == UniformType::FloatMatrix34) ||
            (type == UniformType::FloatMatrix42) ||
            (type == UniformType::FloatMatrix43);
    }

}