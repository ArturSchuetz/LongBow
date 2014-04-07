#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer {

		class OGLFramebufferName
		{
		public:
			OGLFramebufferName();
			~OGLFramebufferName();

			unsigned int GetValue();

		private:
			// You shall not copy"
			OGLFramebufferName(const OGLFramebufferName &obj){}
			unsigned int m_value;
		};

		typedef std::shared_ptr<OGLFramebufferName> OGLFramebufferNamePtr;

	}
}