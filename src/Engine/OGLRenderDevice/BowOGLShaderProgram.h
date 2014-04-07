#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "IBowShaderProgram.h"
#include "IBowCleanableObserver.h"

namespace Bow {
	namespace Renderer {

		using namespace Core;

		typedef std::shared_ptr<class OGLShaderProgramName> OGLShaderProgramNamePtr;
		typedef std::shared_ptr<class OGLShaderObject> OGLShaderObjectPtr;
		typedef std::shared_ptr<class OGLFragmentOutputs> OGLFragmentOutputsPtr;

		class IOGLUniform;
			enum class UniformType : char;
		typedef std::shared_ptr<class IOGLUniform> OGLUniformPtr;
		typedef std::hash_map<std::string, OGLUniformPtr> UniformMap;

		class OGLShaderProgram : public IShaderProgram, public ICleanableObserver
		{
			enum class PrimitiveDatatype : char
			{
				Int = 0,
				Float,
				Uint
			};

		public:
			OGLShaderProgram(const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& fragmentShaderSource);
			~OGLShaderProgram();

			std::string					GetLog();

			ShaderVertexAttributePtr	GetVertexAttribute(std::string name);
			int							GetFragmentOutputLocation(std::string name);

			void SetUniform(const char* name, int value);
			void SetUniform(const char* name, float value);

			void SetUniform(const char* name, int value1, int value2);
			void SetUniform(const char* name, float value1, float value2);

			void SetUniform(const char* name, int value1, int value2, int value3);
			void SetUniform(const char* name, float value1, float value2, float value3);

			void SetUniform(const char* name, int value1, int value2, int value3, int value4);
			void SetUniform(const char* name, float value1, float value2, float value3, float value4);

			void SetUniformVector(const char* name, int* vector, unsigned int vector_size);
			void SetUniformVector(const char* name, float* vector, unsigned int vector_size);

			void SetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size);
			void SetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size);

			// ===========================================
			// New Functions
			// ===========================================
			OGLShaderProgramNamePtr		GetProgram();

			void Bind();
			void Clean();

			bool IsReady();

			void NotifyDirty(ICleanable* value);

		private:
			OGLShaderProgram(OGLShaderProgram&){}
			OGLShaderProgram& operator=(const OGLShaderProgram&) { return *this; }

			void SetUniformI(const char* name, int*, int arrayLength);
			void SetUniformUI(const char* name, unsigned int*, int arrayLength);
			void SetUniformF(const char* name, float*, int arrayLength);
			void SetUniform(const char* name, void*, PrimitiveDatatype, int arrayLength);

			std::string				GetProgramInfoLog();

			ShaderVertexAttributeMap	FindVertexAttributes(OGLShaderProgramNamePtr Program);
			UniformMap					FindUniforms(OGLShaderProgramNamePtr program);

			bool					IsMatrix(UniformType type);

			OGLShaderObjectPtr		m_vertexShader;
			OGLShaderObjectPtr		m_geometryShader;
			OGLShaderObjectPtr		m_fragmentShader;
			OGLShaderProgramNamePtr	m_program;

			ShaderVertexAttributeMap	m_shaderVertexAttributes;
			OGLFragmentOutputsPtr	m_fragmentOutputs;

			UniformMap				m_uniforms;

			std::list<ICleanable*>	m_dirtyUniforms;
			bool m_ready;
		};

		typedef std::shared_ptr<OGLShaderProgram> OGLShaderProgramPtr;

	}
}