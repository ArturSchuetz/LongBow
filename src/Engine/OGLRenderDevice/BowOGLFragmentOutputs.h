#pragma once
#include "BowPrerequisites.h"

#include "IBowFragmentOutputs.h"

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLShaderProgramName> OGLShaderProgramNamePtr;

		class OGLFragmentOutputs : public IFragmentOutputs
		{
		public:
			OGLFragmentOutputs(OGLShaderProgramNamePtr program);
			~OGLFragmentOutputs();

			int operator[](std::string index) const;

		private:
			OGLShaderProgramNamePtr m_program;
		};

		typedef std::shared_ptr<OGLFragmentOutputs> OGLFragmentOutputsPtr;

	}
}