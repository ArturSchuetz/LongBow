#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "IBowShaderProgram.h"
#include "IBowCleanableObserver.h"

namespace bow {

	typedef std::shared_ptr<class OGLShaderProgramName> OGLShaderProgramNamePtr;
	typedef std::shared_ptr<class OGLShaderObject> OGLShaderObjectPtr;
	typedef std::shared_ptr<class OGLFragmentOutputs> OGLFragmentOutputsPtr;

	class IOGLUniform;
		enum class UniformType : char;
	typedef std::shared_ptr<class IOGLUniform> OGLUniformPtr;
	typedef std::unordered_map<std::string, OGLUniformPtr> UniformMap;

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

		// ===========================================
		// IShaderProgram Functions
		// ===========================================

		ShaderVertexAttributePtr	VGetVertexAttribute(std::string name);
		ShaderVertexAttributeMap	VGetVertexAttributes();
		int							VGetFragmentOutputLocation(std::string name);

		void VSetUniform(const char* name, int value);
		void VSetUniform(const char* name, float value);

		void VSetUniform(const char* name, int value1, int value2);
		void VSetUniform(const char* name, float value1, float value2);

		void VSetUniform(const char* name, int value1, int value2, int value3);
		void VSetUniform(const char* name, float value1, float value2, float value3);

		void VSetUniform(const char* name, int value1, int value2, int value3, int value4);
		void VSetUniform(const char* name, float value1, float value2, float value3, float value4);

		void VSetUniformVector(const char* name, int* vector, unsigned int vector_size);
		void VSetUniformVector(const char* name, float* vector, unsigned int vector_size);

		void VSetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size);
		void VSetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size);

		// ===========================================
		// ICleanableObserver Functions
		// ===========================================

		void NotifyDirty(ICleanable* value);

		// ===========================================

		std::string GetLog();
		unsigned int GetProgram();

		void Bind();
		void Clean();

		bool IsReady();

	private:
		OGLShaderProgram(OGLShaderProgram&){}
		OGLShaderProgram& operator=(const OGLShaderProgram&) { return *this; }


		void SetUniformI(const char* name, int*, int arrayLength);
		void SetUniformUI(const char* name, unsigned int*, int arrayLength);
		void SetUniformF(const char* name, float*, int arrayLength);
		void SetUniform(const char* name, void*, PrimitiveDatatype, int arrayLength);

		ShaderVertexAttributeMap	FindVertexAttributes(unsigned int Program);
		UniformMap					FindUniforms(unsigned int program);

		OGLShaderObjectPtr		m_vertexShader;
		OGLShaderObjectPtr		m_geometryShader;
		OGLShaderObjectPtr		m_fragmentShader;

		ShaderVertexAttributeMap	m_shaderVertexAttributes;
		OGLFragmentOutputsPtr	m_fragmentOutputs;

		UniformMap				m_uniforms;

		std::list<ICleanable*>	m_dirtyUniforms;
		bool m_ready;

		unsigned int			m_ShaderProgramHandle;
	};

	typedef std::shared_ptr<OGLShaderProgram> OGLShaderProgramPtr;

}
