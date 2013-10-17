#include "OGLVertexArray.h"
#include "OGLIndexBuffer.h"

#include <GL\glew.h>

namespace Bow {

	OGLVertexArray::OGLVertexArray() : m_dirtyIndexBuffer(false), m_name(OGLVertexArrayNamePtr( new OGLVertexArrayName() ))
	{
		FN("OGLVertexArray::OGLVertexArray()");
    }

	OGLVertexArray::~OGLVertexArray()
    {
		FN("OGLVertexArray::~OGLVertexArray()");
    }

    void OGLVertexArray::Bind()
    {
		FN("OGLVertexArray::Bind()");
        glBindVertexArray(m_name->GetValue());
    }

    void OGLVertexArray::Clean()
    {
		FN("OGLVertexArray::Clean()");
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
		FN("OGLVertexArray::MaximumArrayIndex()");
		return m_attributes.GetMaximumArrayIndex();
    }

	VertexBufferAttributeMap OGLVertexArray::GetAttributes()
    {
		FN("OGLVertexArray::GetAttributes()");
		return m_attributes.GetAttributes();
    }

	void OGLVertexArray::SetAttribute( int Location, VertexBufferAttributePtr pointer )
	{
		m_attributes.SetAttribute(Location, pointer);
	}

    IndexBufferPtr OGLVertexArray::GetIndexBuffer()
    {
		FN("OGLVertexArray::GetIndexBuffer()");
		return std::dynamic_pointer_cast<IIndexBuffer>(m_indexBuffer);
	}

	void OGLVertexArray::SetIndexBuffer(IndexBufferPtr indexBufferPtr)
	{
		FN("OGLVertexArray::SetIndexBuffer()");
		m_indexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(indexBufferPtr);
        m_dirtyIndexBuffer = true;
    }

}