#include "BowOGLUniform.h"
#include "BowLogger.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLUniformF::OGLUniformF(int location, UniformType datatype, int count, ICleanableObserver* observer) : OGLUniform(location, datatype, count, observer)
		{
			switch (Datatype)
			{
			case(UniformType::Float) :
				m_arrayLength = 1 * count;
				break;
			case(UniformType::FloatVector2) :
				m_arrayLength = 2 * count;
				break;
			case(UniformType::FloatVector3) :
				m_arrayLength = 3 * count;
				break;
			case(UniformType::FloatVector4) :
				m_arrayLength = 4 * count;
				break;
			case(UniformType::FloatMatrix22) :
				m_arrayLength = 4 * count;
				break;
			case(UniformType::FloatMatrix23) :
				m_arrayLength = 6 * count;
				break;
			case(UniformType::FloatMatrix24) :
				m_arrayLength = 8 * count;
				break;
			case(UniformType::FloatMatrix32) :
				m_arrayLength = 6 * count;
				break;
			case(UniformType::FloatMatrix33) :
				m_arrayLength = 9 * count;
				break;
			case(UniformType::FloatMatrix34) :
				m_arrayLength = 12 * count;
				break;
			case(UniformType::FloatMatrix42) :
				m_arrayLength = 8 * count;
				break;
			case(UniformType::FloatMatrix43) :
				m_arrayLength = 12 * count;
				break;
			case(UniformType::FloatMatrix44) :
				m_arrayLength = 16 * count;
				break;
			default:
				LOG_FATAL("UniformType does not exist!");
				break;
			}
			m_val = new float[m_arrayLength];
		}

		void OGLUniformF::Clean()
		{
			switch (Datatype)
			{
			case(UniformType::Float) :
				glUniform1fv(Location, m_count, m_val);
				break;
			case(UniformType::FloatVector2) :
				glUniform2fv(Location, m_count, m_val);
				break;
			case(UniformType::FloatVector3) :
				glUniform3fv(Location, m_count, m_val);
				break;
			case(UniformType::FloatVector4) :
				glUniform4fv(Location, m_count, m_val);
				break;
			case(UniformType::FloatMatrix22) :
				glUniformMatrix2fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix23) :
				glUniformMatrix2x3fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix24) :
				glUniformMatrix2x4fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix32) :
				glUniformMatrix3x2fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix33) :
				glUniformMatrix3fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix34) :
				glUniformMatrix3x4fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix42) :
				glUniformMatrix4x2fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix43) :
				glUniformMatrix4x3fv(Location, m_count, GL_FALSE, m_val);
				break;
			case(UniformType::FloatMatrix44) :
				glUniformMatrix4fv(Location, m_count, GL_FALSE, m_val);
				break;
			default:
				LOG_FATAL("UniformType does not exist!");
				break;
			}
		}

	}
}