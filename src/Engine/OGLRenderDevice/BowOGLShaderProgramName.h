#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer {

		class OGLShaderProgramName
		{
		public:
			OGLShaderProgramName();
			~OGLShaderProgramName();

			unsigned int GetValue();
			std::string GetInfoLog();

		private:
			OGLShaderProgramName(const OGLShaderProgramName &obj){}
			unsigned int m_program;
		};

		typedef std::shared_ptr<OGLShaderProgramName> OGLShaderProgramNamePtr;

	}
}