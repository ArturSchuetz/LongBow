#include "BowOGLUniform.h"
#include "BowLogger.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLUniformUI::OGLUniformUI(int location, UniformType datatype, int count, ICleanableObserver* observer) : OGLUniform(location, datatype, count, observer)
		{
			switch (Datatype)
			{
			case(UniformType::Int) :
				m_arrayLength = 1 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			case(UniformType::IntVector2) :
				m_arrayLength = 2 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			case(UniformType::IntVector3) :
				m_arrayLength = 3 * count;
				m_val = new unsigned int[m_arrayLength];
				break;
			case(UniformType::IntVector4) :
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