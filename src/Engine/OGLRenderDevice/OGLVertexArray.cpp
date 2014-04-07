#include "BowOGLVertexArray.h"
#include "BowLogger.h"

#include "BowOGLVertexArrayName.h"
#include "BowOGLIndexBuffer.h"

#include <GL\glew.h>

namespace Bow {
	namespace Renderer {

		OGLVertexArray::OGLVertexArray() : m_dirtyIndexBuffer(false), m_name(OGLVertexArrayNamePtr(new OGLVertexArrayName()))
		{
		}

		OGLVertexArray::~OGLVertexArray()
		{
		}

		void OGLVertexArray::Bind()
		{
			glBindVertexArray(m_name->GetValue());
		}

		void OGLVertexArray::Clean()
		{
			m_attributes.Clean();

			if (m_dirtyIndexBuffer)
			{
				if (m_indexBuffer.get() != nullptr)
				{
					m_indexBuffer->Bind();
				}
				else
				{
					OGLIndexBuffer::UnBind();
				}

				m_dirtyIndexBuffer = false;
			}
		}

		int OGLVertexArray::MaximumArrayIndex()
		{
			return m_attributes.GetMaximumArrayIndex();
		}

		VertexBufferAttributeMap OGLVertexArray::GetAttributes()
		{
			return m_attributes.GetAttributes();
		}

		void OGLVertexArray::SetAttribute(int Location, VertexBufferAttributePtr pointer)
		{
			m_attributes.SetAttribute(Location, pointer);
		}

		IndexBufferPtr OGLVertexArray::GetIndexBuffer()
		{
			return std::dynamic_pointer_cast<IIndexBuffer>(m_indexBuffer);
		}

		void OGLVertexArray::SetIndexBuffer(IndexBufferPtr indexBufferPtr)
		{
			m_indexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(indexBufferPtr);
			m_dirtyIndexBuffer = true;
		}

	}
}