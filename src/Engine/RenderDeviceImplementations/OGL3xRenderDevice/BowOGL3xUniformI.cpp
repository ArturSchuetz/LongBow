#include "BowOGL3xUniform.h"
#include "BowLogger.h"

#include <GL\glew.h>

namespace bow {

	OGLUniformI::OGLUniformI(int location, UniformType datatype, int count, ICleanableObserver* observer) : OGLUniform(location, datatype, count, observer)
	{


		switch (Datatype)
		{
		case(UniformType::Int) :
		case(UniformType::Bool) :
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
			m_arrayLength = 1 * count;
			m_val = new int[m_arrayLength];
			break;
		case(UniformType::IntVector2) :
		case(UniformType::BoolVector2) :
			m_arrayLength = 2 * count;
			m_val = new int[m_arrayLength];
			break;
		case(UniformType::IntVector3) :
		case(UniformType::BoolVector3) :
			m_arrayLength = 3 * count;
			m_val = new int[m_arrayLength];
			break;
		case(UniformType::IntVector4) :
		case(UniformType::BoolVector4) :
			m_arrayLength = 4 * count;
			m_val = new int[m_arrayLength];
			break;
		default:
			LOG_FATAL("UniformType does not exist!");
			break;
		}
		memset(m_val, 0, m_arrayLength*sizeof(int));
	}

	void OGLUniformI::Clean()
	{
		switch (Datatype)
		{
		case(UniformType::Int) :
		case(UniformType::Bool) :
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
			glUniform1iv(Location, m_count, m_val);
			break;
		case(UniformType::IntVector2) :
		case(UniformType::BoolVector2) :
			glUniform2iv(Location, m_count, m_val);
			break;
		case(UniformType::IntVector3) :
		case(UniformType::BoolVector3) :
			glUniform3iv(Location, m_count, m_val);
			break;
		case(UniformType::IntVector4) :
		case(UniformType::BoolVector4) :
			glUniform4iv(Location, m_count, m_val);
			break;
		default:
			LOG_FATAL("UniformType does not exist!");
			break;
		}
	}

}
