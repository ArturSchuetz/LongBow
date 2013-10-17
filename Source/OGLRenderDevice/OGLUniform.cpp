#include "OGLUniform.h"
#include "GL\glew.h"

namespace Bow {

	void NoDeleter(void* ptr) { FN("NoDeleter()"); }
	void ArrayDeleter(void* ptr) { FN("ArrayDeleter()"); delete[] ptr; }

	OGLUniform::OGLUniform(int Location, GLsizei Count, UniformType Type, ICleanableObserver* CleanableObserver) : m_count(Count), m_observer(CleanableObserver), IUniform(Type, Location)
	{
		FN("OGLUniform::OGLUniform()");
		m_value = VoidPtr(nullptr);
		m_dirty = false;
	}


	OGLUniform::~OGLUniform()
	{
		FN("OGLUniform::~OGLUniform()");
	}


	void OGLUniform::SetValue(void* value, bool deletePointer)
	{
		FN("OGLUniform::SetValue()");
		if(!deletePointer)
		{
			SetValue(VoidPtr(value, NoDeleter));
		}
		else
		{
			SetValue(VoidPtr(value));
		}
	}


	void OGLUniform::SetArray(void* value, bool deletePointer)
	{
		FN("OGLUniform::SetArray()");
		if(!deletePointer)
		{
			SetValue(VoidPtr(value, NoDeleter));
		}
		else
		{
			SetValue(VoidPtr(value, ArrayDeleter));
		}
	}


	void OGLUniform::SetValue(VoidPtr value)
	{
		FN("OGLUniform::SetValue()");
		if (!m_dirty)
        {
            m_dirty = true;
			m_observer->NotifyDirty(this);
        }
		m_value = value;
	}


	void OGLUniform::Clean()
	{
		FN("OGLUniform::Clean()");
		if(m_value.get() != nullptr)
		{
			switch(Datatype)
			{
			case UniformType::Float:
				glUniform1fv(Location, m_count, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatVector2:
				glUniform2fv(Location, m_count, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatVector3:
				glUniform3fv(Location, m_count, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatVector4:
				glUniform4fv(Location, m_count, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::Int:
			case UniformType::Bool:
				glUniform1iv(Location, m_count, std::static_pointer_cast<GLint>(m_value).get() );
				break;
			case UniformType::IntVector2:
			case UniformType::BoolVector2:
				glUniform2iv(Location, m_count, std::static_pointer_cast<GLint>(m_value).get() );
				break;
			case UniformType::IntVector3:
			case UniformType::BoolVector3:
				glUniform3iv(Location, m_count, std::static_pointer_cast<GLint>(m_value).get() );
				break;
			case UniformType::IntVector4:
			case UniformType::BoolVector4:
				glUniform4iv(Location, m_count, std::static_pointer_cast<GLint>(m_value).get() );
				break;
			case UniformType::FloatMatrix22:
				glUniformMatrix2fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix33:
				glUniformMatrix3fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix44:
				glUniformMatrix4fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix23:
				glUniformMatrix2x3fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix24:
				glUniformMatrix2x4fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix32:
				glUniformMatrix3x2fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix34:
				glUniformMatrix3x4fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix42:
				glUniformMatrix4x2fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			case UniformType::FloatMatrix43:
				glUniformMatrix4x3fv(Location, m_count, false, std::static_pointer_cast<GLfloat>(m_value).get() );
				break;
			default:
				glUniform1iv(Location, m_count, std::static_pointer_cast<GLint>(m_value).get() );
				break;
			}
		}
		m_dirty = false;
	}

}