#pragma once
#include "LongBow.h"

#include "IBowShaderProgram.h"
#include "OGLShaderObject.h"
#include "OGLShaderProgramName.h"
#include "OGLFragmentOutputs.h"
#include "OGLUniform.h"

namespace Bow {

	class OGLShaderProgram : public IShaderProgram, public ICleanableObserver
	{
	public:
		OGLShaderProgram( const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& fragmentShaderSource);
		~OGLShaderProgram();

		std::string			GetLog();
		
		ShaderVertexAttributePtr	GetVertexAttribute(std::string name);
		int							GetFragmentOutputLocation(std::string name);
		UniformPtr					GetUniform(std::string name);

		// ===========================================
		// New Functions
		// ===========================================
		OGLShaderProgramNamePtr	GetProgram();

		void Bind();
		void Clean();

		bool IsReady();

		void NotifyDirty(ICleanable* value);

	private:
		std::string				GetProgramInfoLog();

		ShaderVertexAttributeMap	FindVertexAttributes(OGLShaderProgramNamePtr Program);
		UniformMap				FindUniforms(OGLShaderProgramNamePtr program);

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