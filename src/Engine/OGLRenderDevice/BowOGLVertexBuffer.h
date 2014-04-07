#pragma once
#include "BowPrerequisites.h"

#include "IBowVertexBuffer.h"
#include "BowOGLBuffer.h"

namespace Bow {
	namespace Renderer {

		class OGLVertexBuffer : public IVertexBuffer
		{
		public:
			OGLVertexBuffer(BufferHint usageHint, int sizeInBytes);
			~OGLVertexBuffer();

			void Bind();
			static void UnBind();

			void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
			std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

			int			GetSizeInBytes();
			BufferHint	GetUsageHint();

		private:
			OGLBuffer	m_bufferObject;
		};

		typedef std::shared_ptr<OGLVertexBuffer> OGLVertexBufferPtr;

	}
}