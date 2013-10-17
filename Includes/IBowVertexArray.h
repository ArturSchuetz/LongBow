#pragma once
#include "LongBow.h"
#include "IBowIndexBuffer.h"
#include "BowVertexBufferAttribute.h"

namespace Bow {

	class IVertexArray
    {
	public:
		virtual VertexBufferAttributeMap GetAttributes() = 0;
		virtual void SetAttribute(int Location, VertexBufferAttributePtr pointer) = 0;

        virtual IndexBufferPtr GetIndexBuffer() = 0;
        virtual void SetIndexBuffer(IndexBufferPtr pointer) = 0;
    };

	typedef std::shared_ptr<IVertexArray> VertexArrayPtr;

}