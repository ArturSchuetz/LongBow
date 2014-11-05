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

			void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
			std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

			int VGetSizeInBytes();
			BufferHint VGetUsageHint();
			IndexBufferDatatype GetDatatype() { return m_Datatype; }

		private:
			OGLBuffer	m_BufferObject;
			IndexBufferDatatype m_Datatype;
		};

		typedef std::shared_ptr<OGLIndexBuffer> OGLIndexBufferPtr;

	}
}