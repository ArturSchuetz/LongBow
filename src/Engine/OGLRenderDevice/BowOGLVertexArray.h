#pragma once
#include "BowPrerequisites.h"

#include "IBowVertexArray.h"

#include "BowOGLVertexBufferAttributes.h"

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLVertexArrayName> OGLVertexArrayNamePtr;
		typedef std::shared_ptr<class OGLIndexBuffer> OGLIndexBufferPtr;

		class OGLVertexArray : public IVertexArray
		{
		public:
			OGLVertexArray();
			~OGLVertexArray();

			void Bind();
			void Clean();

			int	MaximumArrayIndex();

			VertexBufferAttributeMap	GetAttributes();
			void						SetAttribute(int Location, VertexBufferAttributePtr pointer);

			IndexBufferPtr				GetIndexBuffer();
			void						SetIndexBuffer(IndexBufferPtr pointer);

		private:
			OGLVertexArrayNamePtr		m_name;
			OGLVertexBufferAttributes	m_attributes;
			OGLIndexBufferPtr			m_indexBuffer;
			bool						m_dirtyIndexBuffer;
		};

		typedef std::shared_ptr<OGLVertexArray> OGLVertexArrayPtr;

	}
}