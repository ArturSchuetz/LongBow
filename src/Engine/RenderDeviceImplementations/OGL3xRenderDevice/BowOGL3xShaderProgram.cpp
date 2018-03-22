#include "BowOGL3xShaderProgram.h"
#include "BowLogger.h"

#include "BowOGL3xTypeConverter.h"
#include "BowOGL3xShaderObject.h"
#include "BowOGL3xFragmentOutputs.h"
#include "BowOGL3xRenderContext.h"
#include "BowOGL3xUniform.h"

#include "BowShaderVertexAttribute.h"

#include <GL/glew.h>

namespace bow {

	OGLShaderProgram::OGLShaderProgram(const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& fragmentShaderSource) :
		m_ShaderProgramHandle(0)
	{
		m_ShaderProgramHandle = glCreateProgram();
		if (m_ShaderProgramHandle == 0)
			LOG_ERROR("Could not create Shaderobject.");

		m_ready = false;
		m_vertexShader = OGLShaderObjectPtr(new OGLShaderObject(GL_VERTEX_SHADER, vertexShaderSource));
		if (geometryShaderSource.length() > 0)
		{
			m_geometryShader = OGLShaderObjectPtr(new OGLShaderObject(GL_GEOMETRY_SHADER, geometryShaderSource));
		}
		m_fragmentShader = OGLShaderObjectPtr(new OGLShaderObject(GL_FRAGMENT_SHADER, fragmentShaderSource));

		glAttachShader(m_ShaderProgramHandle, m_vertexShader->GetShader());
		if (geometryShaderSource.length() > 0)
		{
			glAttachShader(m_ShaderProgramHandle, m_geometryShader->GetShader());
		}
		glAttachShader(m_ShaderProgramHandle, m_fragmentShader->GetShader());

		glLinkProgram(m_ShaderProgramHandle);

		int linkStatus;
		glGetProgramiv(m_ShaderProgramHandle, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == 0)
		{
			LOG_ERROR(VGetLog().c_str());
			m_ready = false;
		}
		else
		{
			LOG_TRACE("Shader successfully linked!");
			m_ready = true;
		}

		m_fragmentOutputs = OGLFragmentOutputsPtr(new OGLFragmentOutputs(m_ShaderProgramHandle));
		m_shaderVertexAttributes = FindVertexAttributes(m_ShaderProgramHandle);

		m_uniforms = FindUniforms(m_ShaderProgramHandle);
	}


	OGLShaderProgram::~OGLShaderProgram()
	{
		glDeleteProgram(m_ShaderProgramHandle);
	}


	std::string	OGLShaderProgram::VGetLog()
	{
		char* buffer;
		GLint length, result;

		/* get the shader info log */
		glGetProgramiv(m_ShaderProgramHandle, GL_INFO_LOG_LENGTH, &length);
		buffer = (char*)malloc(length);

		glGetProgramInfoLog(m_ShaderProgramHandle, length, &result, buffer);

		return std::string(buffer);
	}


	ShaderVertexAttributePtr OGLShaderProgram::VGetVertexAttribute(std::string name)
	{
		return m_shaderVertexAttributes[name];
	}


	ShaderVertexAttributeMap OGLShaderProgram::VGetVertexAttributes()
	{
		return m_shaderVertexAttributes;
	}


	int OGLShaderProgram::VGetFragmentOutputLocation(std::string name)
	{
		return (*m_fragmentOutputs)[name];
	}


	unsigned int OGLShaderProgram::GetProgram()
	{
		return m_ShaderProgramHandle;
	}


	void OGLShaderProgram::Bind()
	{
		glUseProgram(m_ShaderProgramHandle);
	}


	void OGLShaderProgram::Clean()
	{
		for (auto it = m_dirtyUniforms.begin(); it != m_dirtyUniforms.end(); it++)
		{
			(*it)->Clean();
		}
		m_dirtyUniforms.clear();
	}


	bool OGLShaderProgram::IsReady()
	{
		return m_ready;
	}


	void OGLShaderProgram::NotifyDirty(ICleanable* value)
	{
		m_dirtyUniforms.push_back(value);
	}

	// =====================================================================
	// PRIVATE FUNCTIONS
	// =====================================================================

	ShaderVertexAttributeMap OGLShaderProgram::FindVertexAttributes(unsigned int programHandle)
	{
		int numberOfAttributes;
		glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &numberOfAttributes);

		int attributeNameMaxLength;
		glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeNameMaxLength);

		ShaderVertexAttributeMap vertexAttributes = ShaderVertexAttributeMap();

		if (numberOfAttributes > 0)
			LOG_TRACE("\tVertexAttributes:");

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

			LOG_TRACE("\t\tName: %s, \tLocation: %d", attributeName, attributeLocation);
			vertexAttributes.insert(std::pair<std::string, ShaderVertexAttributePtr>(attributeName, ShaderVertexAttributePtr(new ShaderVertexAttribute(attributeLocation, OGLTypeConverter::ToActiveAttribType(attributeType), attributeLength))));
		}
		return vertexAttributes;
	}


	UniformMap OGLShaderProgram::FindUniforms(unsigned int programHandle)
	{
		int numberOfUniforms;
		glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

		int uniformNameMaxLength;
		glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameMaxLength);

		UniformMap uniforms;
		if (numberOfUniforms > 0)
			LOG_TRACE("\tUniforms:");

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

			LOG_TRACE("\t\tName: %s, \tLocation: %d, \tArraySize: %d", uniformName, uniformLocation, uniformSize);

			UniformType type = OGLTypeConverter::ToActiveUniformType(uniformType);
			switch (type)
			{
				case(UniformType::Float) :
				case(UniformType::FloatVector2) :
				case(UniformType::FloatVector3) :
				case(UniformType::FloatVector4) :
				case(UniformType::FloatMatrix22) :
				case(UniformType::FloatMatrix23) :
				case(UniformType::FloatMatrix24) :
				case(UniformType::FloatMatrix32) :
				case(UniformType::FloatMatrix33) :
				case(UniformType::FloatMatrix34) :
				case(UniformType::FloatMatrix44) :
					uniforms.insert(std::pair<std::string, OGLUniformPtr>(uniformName, OGLUniformPtr(new OGLUniformF(uniformLocation, type, uniformSize, this))));
					break;
				case(UniformType::Bool) :
				case(UniformType::BoolVector2) :
				case(UniformType::BoolVector3) :
				case(UniformType::BoolVector4) :
				case(UniformType::Int) :
				case(UniformType::IntVector2) :
				case(UniformType::IntVector3) :
				case(UniformType::IntVector4) :
				case(UniformType::IntSampler1D) :
				case(UniformType::IntSampler2D) :
				case(UniformType::IntSampler2DRectangle) :
				case(UniformType::IntSampler3D) :
				case(UniformType::IntSamplerCube) :
				case(UniformType::IntSampler1DArray) :
				case(UniformType::IntSampler2DArray) :
				case(UniformType::Sampler1D) :
				case(UniformType::Sampler2D) :
				case(UniformType::Sampler2DRectangle) :
				case(UniformType::Sampler2DRectangleShadow) :
				case(UniformType::Sampler3D) :
				case(UniformType::SamplerCube) :
				case(UniformType::Sampler1DShadow) :
				case(UniformType::Sampler2DShadow) :
				case(UniformType::Sampler1DArray) :
				case(UniformType::Sampler2DArray) :
				case(UniformType::Sampler1DArrayShadow) :
				case(UniformType::Sampler2DArrayShadow) :
				case(UniformType::SamplerCubeShadow) :
					uniforms.insert(std::pair<std::string, OGLUniformPtr>(uniformName, OGLUniformPtr(new OGLUniformI(uniformLocation, type, uniformSize, this))));
					break;
				case(UniformType::UnsignedInt) :
				case(UniformType::UnsignedIntVector2) :
				case(UniformType::UnsignedIntVector3) :
				case(UniformType::UnsignedIntVector4) :
				case(UniformType::UnsignedIntSampler1D) :
				case(UniformType::UnsignedIntSampler2D) :
				case(UniformType::UnsignedIntSampler2DRectangle) :
				case(UniformType::UnsignedIntSampler3D) :
				case(UniformType::UnsignedIntSamplerCube) :
				case(UniformType::UnsignedIntSampler1DArray) :
				case(UniformType::UnsignedIntSampler2DArray) :
					uniforms.insert(std::pair<std::string, OGLUniformPtr>(uniformName, OGLUniformPtr(new OGLUniformUI(uniformLocation, type, uniformSize, this))));
					break;
				default:
					LOG_FATAL("UniformType does not exist!");
			}
		}
		return uniforms;
	}


	static bool IsMatrix(UniformType type)
	{
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

	void OGLShaderProgram::VSetUniform(const char* name, int val1)
	{
		int val[1] = { val1 };
		SetUniformI(name, val, 1);
	}

	void OGLShaderProgram::VSetUniform(const char* name, float val1)
	{
		float val[1] = { val1 };
		SetUniformF(name, val, 1);
	}

	void OGLShaderProgram::VSetUniform(const char* name, int val1, int val2)
	{
		int val[2] = { val1, val2 };
		SetUniformI(name, val, 2);
	}

	void OGLShaderProgram::VSetUniform(const char* name, float val1, float val2)
	{
		float val[2] = { val1, val2 };
		SetUniformF(name, val, 2);
	}

	void OGLShaderProgram::VSetUniform(const char* name, int val1, int val2, int val3)
	{
		int val[3] = { val1, val2, val3 };
		SetUniformI(name, val, 3);
	}

	void OGLShaderProgram::VSetUniform(const char* name, float val1, float val2, float val3)
	{
		float val[3] = { val1, val2, val3 };
		SetUniformF(name, val, 3);
	}

	void OGLShaderProgram::VSetUniform(const char* name, int val1, int val2, int val3, int val4)
	{
		int val[4] = { val1, val2, val3, val4 };
		SetUniformI(name, val, 4);
	}

	void OGLShaderProgram::VSetUniform(const char* name, float val1, float val2, float val3, float val4)
	{
		float val[4] = { val1, val2, val3, val4 };
		SetUniformF(name, val, 4);
	}

	void OGLShaderProgram::VSetUniformVector(const char* name, int* val, unsigned int array_count)
	{
		SetUniformI(name, val, array_count);
	}
	void OGLShaderProgram::VSetUniformVector(const char* name, float* val, unsigned int array_count)
	{
		SetUniformF(name, val, array_count);
	}

	void OGLShaderProgram::VSetUniformMatrix(const char* name, int* val, unsigned int array_count)
	{
		SetUniformI(name, val, array_count);
	}

	void OGLShaderProgram::VSetUniformMatrix(const char* name, float* val, unsigned int array_coun)
	{
		SetUniformF(name, val, array_coun);
	}

	void OGLShaderProgram::SetUniformI(const char* name, int* val, int arrayLength)
	{
		SetUniform(name, val, OGLShaderProgram::PrimitiveDatatype::Int, arrayLength);
	}

	void OGLShaderProgram::SetUniformF(const char* name, float* val, int arrayLength)
	{
		SetUniform(name, val, OGLShaderProgram::PrimitiveDatatype::Float, arrayLength);
	}

	void OGLShaderProgram::SetUniformUI(const char* name, unsigned int* val, int arrayLength)
	{
		SetUniform(name, val, OGLShaderProgram::PrimitiveDatatype::Uint, arrayLength);
	}

	void OGLShaderProgram::SetUniform(const char* name, void* val, PrimitiveDatatype type, int arrayLength)
	{
		auto it = m_uniforms.find(name);
		if (it != m_uniforms.end())
		{
			switch (m_uniforms[name]->Datatype)
			{
			case(UniformType::Float) :
			case(UniformType::FloatVector2) :
			case(UniformType::FloatVector3) :
			case(UniformType::FloatVector4) :
			case(UniformType::FloatMatrix22) :
			case(UniformType::FloatMatrix23) :
			case(UniformType::FloatMatrix24) :
			case(UniformType::FloatMatrix32) :
			case(UniformType::FloatMatrix33) :
			case(UniformType::FloatMatrix34) :
			case(UniformType::FloatMatrix44) :
			switch (type)
				{
					case(OGLShaderProgram::PrimitiveDatatype::Float) :
						(std::dynamic_pointer_cast<OGLUniformF>(m_uniforms[name]))->SetValue(static_cast<float*>(val), arrayLength);
						break;
					case(OGLShaderProgram::PrimitiveDatatype::Int) :
						(std::dynamic_pointer_cast<OGLUniformF>(m_uniforms[name]))->SetValue(static_cast<int*>(val), arrayLength);
						break;
					case(OGLShaderProgram::PrimitiveDatatype::Uint) :
						(std::dynamic_pointer_cast<OGLUniformF>(m_uniforms[name]))->SetValue(static_cast<unsigned int*>(val), arrayLength);
						break;
				}
				break;
			case(UniformType::Bool) :
			case(UniformType::BoolVector2) :
			case(UniformType::BoolVector3) :
			case(UniformType::BoolVector4) :
			case(UniformType::Int) :
			case(UniformType::IntVector2) :
			case(UniformType::IntVector3) :
			case(UniformType::IntVector4) :
			case(UniformType::IntSampler1D) :
			case(UniformType::IntSampler2D) :
			case(UniformType::IntSampler2DRectangle) :
			case(UniformType::IntSampler3D) :
			case(UniformType::IntSamplerCube) :
			case(UniformType::IntSampler1DArray) :
			case(UniformType::IntSampler2DArray) :
			case(UniformType::Sampler1D) :
			case(UniformType::Sampler2D) :
			case(UniformType::Sampler2DRectangle) :
			case(UniformType::Sampler2DRectangleShadow) :
			case(UniformType::Sampler3D) :
			case(UniformType::SamplerCube) :
			case(UniformType::Sampler1DShadow) :
			case(UniformType::Sampler2DShadow) :
			case(UniformType::Sampler1DArray) :
			case(UniformType::Sampler2DArray) :
			case(UniformType::Sampler1DArrayShadow) :
			case(UniformType::Sampler2DArrayShadow) :
			case(UniformType::SamplerCubeShadow) :
			switch (type)
				{
					case(OGLShaderProgram::PrimitiveDatatype::Float) :
						(std::dynamic_pointer_cast<OGLUniformI>(m_uniforms[name]))->SetValue(static_cast<float*>(val), arrayLength);
						break;
					case(OGLShaderProgram::PrimitiveDatatype::Int) :
						(std::dynamic_pointer_cast<OGLUniformI>(m_uniforms[name]))->SetValue(static_cast<int*>(val), arrayLength);
						break;
					case(OGLShaderProgram::PrimitiveDatatype::Uint) :
						(std::dynamic_pointer_cast<OGLUniformI>(m_uniforms[name]))->SetValue(static_cast<unsigned int*>(val), arrayLength);
							break;
				}
				break;
			case(UniformType::UnsignedInt) :
			case(UniformType::UnsignedIntVector2) :
			case(UniformType::UnsignedIntVector3) :
			case(UniformType::UnsignedIntVector4) :
			case(UniformType::UnsignedIntSampler1D) :
			case(UniformType::UnsignedIntSampler2D) :
			case(UniformType::UnsignedIntSampler2DRectangle) :
			case(UniformType::UnsignedIntSampler3D) :
			case(UniformType::UnsignedIntSamplerCube) :
			case(UniformType::UnsignedIntSampler1DArray) :
			case(UniformType::UnsignedIntSampler2DArray) :
			switch (type)
				{
					case(OGLShaderProgram::PrimitiveDatatype::Float) :
						(std::dynamic_pointer_cast<OGLUniformUI>(m_uniforms[name]))->SetValue(static_cast<float*>(val), arrayLength);
						break;
					case(OGLShaderProgram::PrimitiveDatatype::Int) :
						(std::dynamic_pointer_cast<OGLUniformUI>(m_uniforms[name]))->SetValue(static_cast<int*>(val), arrayLength);
						break;
					case(OGLShaderProgram::PrimitiveDatatype::Uint) :
						(std::dynamic_pointer_cast<OGLUniformUI>(m_uniforms[name]))->SetValue(static_cast<unsigned int*>(val), arrayLength);
						break;
				}
				break;
			default:
				LOG_FATAL("UniformType does not exist!");
				break;
			};
		}
	}

}
