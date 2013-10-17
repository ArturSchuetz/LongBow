#pragma once
#include "LongBow.h"
#include "IBowIndexBuffer.h"
#include "OGLBuffer.h"

namespace Bow {

	class OGLIndexBuffer : public IIndexBuffer
    {
	public:
		OGLIndexBuffer(BufferHint usageHint, int sizeInBytes);
        ~OGLIndexBuffer();

		void Bind();
        static void UnBind();
        int GetCount();

        void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
        VoidPtr CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

		int GetSizeInBytes();
        BufferHint GetUsageHint();

	private:
        OGLBuffer			m_bufferObject;
    };

	typedef std::shared_ptr<OGLIndexBuffer> OGLIndexBufferPtr;
}