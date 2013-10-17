#pragma once
#include "LongBow.h"
#include "BowBufferHint.h"
#include "OGLBufferName.h"

namespace Bow {

	class OGLBuffer
    {
	public:
		OGLBuffer(unsigned int type, BufferHint usageHint, int sizeInBytes);
        ~OGLBuffer();

		void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		VoidPtr CopyToSystemMemory(int offsetInBytes, int lengthInBytes);

		int GetSizeInBytes();
        BufferHint GetUsageHint();
        OGLBufferNamePtr GetHandle();

        void Bind();

	private:
		OGLBufferNamePtr m_name;
        const int m_sizeInBytes;
        const unsigned int m_type;
        const unsigned int m_usageHint;
    };

	typedef std::shared_ptr<OGLBuffer> OGLBufferPtr;
}