#include "BowOGLShaderProgram.h"
#include "BowLogger.h"

#include "BowOGLTypeConverter.h"
#include "BowOGLShaderProgramName.h"
#include "BowOGLShaderObject.h"
#include "BowOGLFragmentOutputs.h"
#include "BowOGLRenderContext.h"
#include "BowOGLUniform.h"

#include "BowShaderVertexAttribute.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLShaderProgram::OGLShaderProgram(const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& fragmentShaderSource)
		{
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
				LOG_DEBUG("Shader successfully linked!");
				m_ready = true;
			}

			m_fragmentOutputs = OGLFragmentOutputsPtr(new OGLFragmentOutputs(m_program));
			m_shaderVertexAttributes = FindVertexAttributes(m_program);

			m_uniforms = FindUniforms(m_program);
		}


		OGLShaderProgram::~OGLShaderProgram()
		{
		}


		std::string	OGLShaderProgram::GetLog()
		{
			return GetProgramInfoLog();
		}


		ShaderVertexAttributePtr OGLShaderProgram::GetVertexAttribute(std::string name)
		{
			return m_shaderVertexAttributes[name];
		}


		int OGLShaderProgram::GetFragmentOutputLocation(std::string name)
		{
			return (*m_fragmentOutputs)[name];
		}


		OGLShaderProgramNamePtr	OGLShaderProgram::GetProgram()
		{
			LOG_ASSERT(m_program.get() != nullptr, "OGLShaderProgramNamePtr is a nullptr");
			return m_program;
		}


		void OGLShaderProgram::Bind()
		{
			glUseProgram(m_program->GetValue());
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

		std::string	OGLShaderProgram::GetProgramInfoLog()
		{
			LOG_ASSERT(m_program.get() != nullptr, "OGLShaderProgramNamePtr is a nullptr.");

			return m_program->GetInfoLog();
		}


		ShaderVertexAttributeMap OGLShaderProgram::FindVertexAttributes(OGLShaderProgramNamePtr Program)
		{
			int programHandle = Program->GetValue();

			int numberOfAttributes;
			glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &numberOfAttributes);

			int attributeNameMaxLength;
			glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeNameMaxLength);

			ShaderVertexAttributeMap vertexAttributes = ShaderVertexAttributeMap();

			if (numberOfAttributes > 0)
				LOG_DEBUG("\tVertexAttributes:");

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

				LOG_DEBUG("\t\tName: %s, \tLocation: %d", attributeName, attributeLocation);
				vertexAttributes.insert(std::pair<std::string, ShaderVertexAttributePtr>(attributeName, ShaderVertexAttributePtr(new ShaderVertexAttribute(attributeLocation, OGLTypeConverter::ToActiveAttribType(attributeType), attributeLength))));
			}
			return vertexAttributes;
		}


		UniformMap OGLShaderProgram::FindUniforms(OGLShaderProgramNamePtr program)
		{
			int programHandle = program->GetValue();

			int numberOfUniforms;
			glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

			int uniformNameMaxLength;
			glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameMaxLength);

			UniformMap uniforms;
			if (numberOfUniforms > 0)
				LOG_DEBUG("\tUniforms:");

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

				LOG_DEBUG("\t\tName: %s, \tLocation: %d, \tArraySize: %d", uniformName, uniformLocation, uniformSize);
				//uniforms.insert(std::pair<std::string, UniformPtr>(uniformName, UniformPtr(new OGLUniform(uniformLocation, uniformSize, OGLTypeConverter::ToActiveUniformType(uniformType), this))));
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
				case(UniformType::Int) :
				case(UniformType::IntVector2) :
				case(UniformType::IntVector3) :
				case(UniformType::IntVector4) :
											  uniforms.insert(std::pair<std::string, OGLUniformPtr>(uniformName, OGLUniformPtr(new OGLUniformI(uniformLocation, type, uniformSize, this))));
					break;
				case(UniformType::UnsignedInt) :
				case(UniformType::UnsignedIntVector2) :
				case(UniformType::UnsignedIntVector3) :
				case(UniformType::UnsignedIntVector4) :
													  uniforms.insert(std::pair<std::string, OGLUniformPtr>(uniformName, OGLUniformPtr(new OGLUniformUI(uniformLocation, type, uniformSize, this))));
					break;
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

		void OGLShaderProgram::SetUniform(const char* name, int val)
		{
			SetUniformI(name, &val, 1);
		}

		void OGLShaderProgram::SetUniform(const char* name, float val)
		{
			SetUniformF(name, &val, 2);
		}

		void OGLShaderProgram::SetUniform(const char* name, int val1, int val2)
		{
			int val[2] = { val1, val2 };
			SetUniformI(name, val, 2);
		}

		void OGLShaderProgram::SetUniform(const char* name, float val1, float val2)
		{
			float val[2] = { val1, val2 };
			SetUniformF(name, val, 2);
		}

		void OGLShaderProgram::SetUniform(const char* name, int val1, int val2, int val3)
		{
			int val[3] = { val1, val2, val3 };
			SetUniformI(name, val, 3);
		}

		void OGLShaderProgram::SetUniform(const char* name, float val1, float val2, float val3)
		{
			float val[3] = { val1, val2, val3 };
			SetUniformF(name, val, 3);
		}

		void OGLShaderProgram::SetUniform(const char* name, int val1, int val2, int val3, int val4)
		{
			int val[4] = { val1, val2, val3, val4 };
			SetUniformI(name, val, 4);
		}

		void OGLShaderProgram::SetUniform(const char* name, float val1, float val2, float val3, float val4)
		{
			float val[4] = { val1, val2, val3, val4 };
			SetUniformF(name, val, 4);
		}

		void OGLShaderProgram::SetUniformVector(const char* name, int* val, unsigned int array_count)
		{
			SetUniformI(name, val, array_count);
		}
		void OGLShaderProgram::SetUniformVector(const char* name, float* val, unsigned int array_count)
		{
			SetUniformF(name, val, array_count);
		}

		void OGLShaderProgram::SetUniformMatrix(const char* name, int* val, unsigned int array_count)
		{
			SetUniformI(name, val, array_count);
		}

		void OGLShaderProgram::SetUniformMatrix(const char* name, float* val, unsigned int array_coun)
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
				case(UniformType::Int) :
				case(UniformType::IntVector2) :
				case(UniformType::IntVector3) :
				case(UniformType::IntVector4) :
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
}