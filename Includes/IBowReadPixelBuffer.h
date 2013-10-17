#pragma once
#include "LongBow.h"
#include "BowPixelBufferHint.h"

namespace Bow {

    class IReadPixelBuffer
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

        virtual int				GetSizeInBytes() const = 0;
        virtual PixelBufferHint	GetUsageHint() const = 0;
    };

	typedef std::shared_ptr<IReadPixelBuffer> ReadPixelBufferPtr;

}