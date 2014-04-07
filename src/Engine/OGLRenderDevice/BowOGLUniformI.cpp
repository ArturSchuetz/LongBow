#include "BowOGLUniform.h"
#include "BowLogger.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLUniformI::OGLUniformI(int location, UniformType datatype, int count, ICleanableObserver* observer) : OGLUniform(location, datatype, count, observer)
		{
			switch (Datatype)
			{
			case(UniformType::Int) :
				m_arrayLength = 1 * count;
				m_val = new int[m_arrayLength];
				break;
			case(UniformType::IntVector2) :
				m_arrayLength = 2 * count;
				m_val = new int[m_arrayLength];
				break;
			case(UniformType::IntVector3) :
				m_arrayLength = 3 * count;
				m_val = new int[m_arrayLength];
				break;
			case(UniformType::IntVector4) :
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
				glUniform1iv(Location, m_count, m_val);
				break;
			case(UniformType::IntVector2) :
				glUniform2iv(Location, m_count, m_val);
				break;
			case(UniformType::IntVector3) :
				glUniform3iv(Location, m_count, m_val);
				break;
			case(UniformType::IntVector4) :
				glUniform4iv(Location, m_count, m_val);
				break;
			default:
				LOG_FATAL("UniformType does not exist!");
				break;
			}
		}

	}
}