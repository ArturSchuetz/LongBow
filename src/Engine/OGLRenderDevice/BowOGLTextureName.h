#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer {

		class OGLTextureName
		{
		public:
			OGLTextureName();
			~OGLTextureName();

			unsigned int GetValue();

		private:
			OGLTextureName(const OGLTextureName &obj){}
			unsigned int m_value;
		};

		typedef std::shared_ptr<OGLTextureName> OGLTextureNamePtr;

	}
}