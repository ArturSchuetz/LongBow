#pragma once
#include "LongBow.h"
#include "BowBufferHint.h"
#include "BowIndexBufferDatatype.h"

namespace Bow {

    class IIndexBuffer
    {
	public:
		virtual void CopyFromSystemMemory(void* bufferInSystemMemory, int lengthInBytes)
        {
            CopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes);
        }

        virtual void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes) = 0;

        virtual VoidPtr CopyToSystemMemory()
        {
            return CopyToSystemMemory(0, GetSizeInBytes());
        }

        virtual VoidPtr CopyToSystemMemory(int offsetInBytes, int sizeInBytes) = 0;

		virtual int					GetSizeInBytes() = 0;
        virtual BufferHint			GetUsageHint() = 0;
    };

	typedef std::shared_ptr<IIndexBuffer> IndexBufferPtr;

}