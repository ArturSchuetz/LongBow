#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer {

		class OGLVertexArrayName
		{
		public:
			OGLVertexArrayName();
			~OGLVertexArrayName();

			unsigned int GetValue();

		private:
			// You shall not copy"
			OGLVertexArrayName(const OGLVertexArrayName &obj){}
			unsigned int m_value;
		};

		typedef std::shared_ptr<OGLVertexArrayName> OGLVertexArrayNamePtr;

	}
}