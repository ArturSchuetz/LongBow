#pragma once
#include "BowPrerequisites.h"

#include "IBowFragmentOutputs.h"

namespace bow {

	class OGLFragmentOutputs : public IFragmentOutputs
	{
	public:
		OGLFragmentOutputs(unsigned int program);
		~OGLFragmentOutputs();

		int operator[](std::string index) const;

	private:
		unsigned int m_ShaderProgramHandle;
	};

	typedef std::shared_ptr<OGLFragmentOutputs> OGLFragmentOutputsPtr;

}
