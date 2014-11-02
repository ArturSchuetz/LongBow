#pragma once
#include "BowPrerequisites.h"

#include "IBowIndexBuffer.h"
#include "BowOGLBuffer.h"
#include "BowIndexBufferDatatype.h"

namespace Bow {
	namespace Renderer {

		class OGLIndexBuffer : public IIndexBuffer
		{
		public:
			OGLIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes);
			~OGLIndexBuffer();

			void Bind();
			static void UnBind();
			int GetCount();

			void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
			std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

			int GetSizeInBytes();
			BufferHint GetUsageHint();
			IndexBufferDatatype GetDatatype() { return m_Datatype; }

		private:
			OGLBuffer	m_bufferObject;
			IndexBufferDatatype m_Datatype;
		};

		typedef std::shared_ptr<OGLIndexBuffer> OGLIndexBufferPtr;

	}
}