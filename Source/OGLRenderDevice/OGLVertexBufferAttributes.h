#pragma once
#include "LongBow.h"
#include "BowVertexBufferAttribute.h"

#include <GL\glew.h>

namespace Bow {

	struct OGLVertexBufferAttribute
    {
	public:
		VertexBufferAttributePtr VertexBufferAttribute;
		bool Dirty;
    };

	typedef std::hash_map<int, OGLVertexBufferAttribute> OGLVertexBufferAttributeMap;

	class OGLVertexBufferAttributes
	{
	public:
		OGLVertexBufferAttributes() : m_count(0), m_maximumArrayIndex(0), m_dirty(false) {}
		VertexBufferAttributeMap GetAttributes();

		VertexBufferAttributePtr GetAttribute(int index);
		void SetAttribute(int Location, VertexBufferAttributePtr attribute);

		void Clean();
		int GetMaximumArrayIndex();

	private:
		void Attach(int index);
        static void Detach(int index);

        static inline int NumberOfVertices(VertexBufferAttributePtr attribute)
        {
			FN("OGLVertexBufferAttributes::NumberOfVertices()");
			return attribute->GetVertexBuffer()->GetSizeInBytes() / attribute->GetStrideInBytes();
        }

	private:
        OGLVertexBufferAttributeMap m_attributes;
        int							m_count;
        int							m_maximumArrayIndex;
        bool						m_dirty;
	};
}