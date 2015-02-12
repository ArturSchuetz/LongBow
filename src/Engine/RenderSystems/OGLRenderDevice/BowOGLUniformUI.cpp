#include "BowOGLUniform.h"
#include "BowLogger.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLUniformUI::OGLUniformUI(int location, UniformType datatype, int count, ICleanableObserver* observer) : OGLUniform(location, datatype, count, observer)
		{

			switch (Datatype)
			{
			case(UniformType::UnsignedInt) :
			case(UniformType::UnsignedIntSampler1D) :
			case(UniformType::UnsignedIntSampler2D) :
			case(UniformType::UnsignedIntSampler2DRectangle) :
			case(UniformType::UnsignedIntSampler3D) :
			case(UniformType::UnsignedIntSamplerCube) :
			case(UniformType::UnsignedIntSampler1DArray) :
			case(UniformType::UnsignedIntSampler2DArray) :
				m_arrayLength = 1 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			case(UniformType::UnsignedIntVector2) :
				m_arrayLength = 2 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			case(UniformType::UnsignedIntVector3) :
				m_arrayLength = 3 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			case(UniformType::UnsignedIntVector4) :
				m_arrayLength = 4 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			default:
				LOG_FATAL("UniformType does not exist!");
				break;
			}
		}

		void OGLUniformUI::Clean()
		{
			switch (Datatype)
			{
			case(UniformType::UnsignedInt) :
			case(UniformType::UnsignedIntSampler1D) :
			case(UniformType::UnsignedIntSampler2D) :
			case(UniformType::UnsignedIntSampler2DRectangle) :
			case(UniformType::UnsignedIntSampler3D) :
			case(UniformType::UnsignedIntSamplerCube) :
			case(UniformType::UnsignedIntSampler1DArray) :
			case(UniformType::UnsignedIntSampler2DArray) :
				glUniform1uiv(Location, m_count, m_val);
				break;
			case(UniformType::UnsignedIntVector2) :
				glUniform2uiv(Location, m_count, m_val);
				break;
			case(UniformType::UnsignedIntVector3) :
				glUniform3uiv(Location, m_count, m_val);
				break;
			case(UniformType::UnsignedIntVector4) :
				glUniform4uiv(Location, m_count, m_val);
				break;
			default:
				LOG_FATAL("UniformType does not exist!");
				break;
			}
		}

	}
}