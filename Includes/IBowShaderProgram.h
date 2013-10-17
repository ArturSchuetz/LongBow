#pragma once
#include "LongBow.h"
#include "IBowShaderVertexAttribute.h"
#include "IBowFragmentOutputs.h"
#include "IBowUniform.h"

namespace Bow {


	class IShaderProgram
	{
	public:
		virtual ~IShaderProgram(){}

		virtual std::string					GetLog() = 0;
		virtual ShaderVertexAttributePtr	GetVertexAttribute(std::string name) = 0;
		virtual int							GetFragmentOutputLocation(std::string name) = 0;
		virtual UniformPtr					GetUniform(std::string name) = 0;
	};

	typedef std::shared_ptr<IShaderProgram> ShaderProgramPtr;

}