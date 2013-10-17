#pragma once
#include "IBowUniform.h"
#include "ICleanable.h"
#include "ICleanableObserver.h"

#include <GL\glew.h>

namespace Bow {

	class OGLUniform : public IUniform, public ICleanable
    {
	public:
		OGLUniform(int Location, GLsizei Count, UniformType Type, ICleanableObserver* CleanableObserver);
		~OGLUniform();

		UniformType GetDatatype() const;
		int GetLocation() const;

		void SetValue(void* value, bool deletePointer);
		void SetArray(void* value, bool deleteArray);

		void Clean();

	private:
		void SetValue(VoidPtr value);

		GLsizei     m_count;
		VoidPtr		m_value;

		bool		m_dirty;
		ICleanableObserver* const m_observer;
    };

	typedef std::shared_ptr<OGLUniform> OGLUniformPtr;

}