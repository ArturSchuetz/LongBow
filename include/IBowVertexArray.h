#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class IVertexArray
		{
		public:
			virtual ~IVertexArray() {}
			virtual VertexBufferAttributeMap GetAttributes() = 0;
			virtual void VSetAttribute(int Location, VertexBufferAttributePtr pointer) = 0;

			virtual IndexBufferPtr	GetIndexBuffer() = 0;
			virtual void			SetIndexBuffer(IndexBufferPtr pointer) = 0;
		};

	}
}