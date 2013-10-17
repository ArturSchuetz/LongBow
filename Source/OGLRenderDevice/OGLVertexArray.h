#pragma once
#include "IBowVertexArray.h"
#include "OGLVertexArrayName.h"
#include "OGLVertexBufferAttributes.h"
#include "OGLVertexBuffer.h"
#include "OGLIndexBuffer.h"

namespace Bow {

	class OGLVertexArray : public IVertexArray
    {
	public:
		OGLVertexArray();
		~OGLVertexArray();

        void Bind();
        void Clean();

        int	MaximumArrayIndex();

		VertexBufferAttributeMap	GetAttributes();
		void						SetAttribute( int Location, VertexBufferAttributePtr pointer );

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