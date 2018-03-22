#pragma once
#include "BowPrerequisites.h"

#include "IBowVertexArray.h"

#include "BowOGL3xVertexBufferAttributes.h"

namespace bow {

	typedef std::shared_ptr<class OGLIndexBuffer> OGLIndexBufferPtr;

	class OGLVertexArray : public IVertexArray
	{
	public:
		OGLVertexArray();
		~OGLVertexArray();

		void Bind();
		void Clean();

		int	MaximumArrayIndex();

		VertexBufferAttributeMap	VGetAttributes();
		void						VSetAttribute(unsigned int location, VertexBufferAttributePtr pointer);
		void						VSetAttribute(ShaderVertexAttributePtr Location, VertexBufferAttributePtr pointer);

		IndexBufferPtr				VGetIndexBuffer();
		void						VSetIndexBuffer(IndexBufferPtr pointer);

	private:
		OGLVertexBufferAttributes	m_Attributes;
		OGLIndexBufferPtr			m_indexBuffer;
		bool						m_dirtyIndexBuffer;

		unsigned int				m_VertexArrayHandle;
	};

	typedef std::shared_ptr<OGLVertexArray> OGLVertexArrayPtr;

}
