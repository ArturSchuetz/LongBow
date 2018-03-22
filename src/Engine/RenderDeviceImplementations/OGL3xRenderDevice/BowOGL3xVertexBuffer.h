#pragma once
#include "BowPrerequisites.h"

#include "IBowVertexBuffer.h"
#include "BowOGL3xBuffer.h"

namespace bow {

	class OGLVertexBuffer : public IVertexBuffer
	{
	public:
		OGLVertexBuffer(BufferHint usageHint, int sizeInBytes);
		~OGLVertexBuffer();

		void Bind();
		static void UnBind();

		void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

		int			VGetSizeInBytes();
		BufferHint	VGetUsageHint();

	private:
		OGLBuffer	m_BufferObject;
	};

	typedef std::shared_ptr<OGLVertexBuffer> OGLVertexBufferPtr;

}
