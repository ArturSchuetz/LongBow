#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer{

		class IReadPixelBuffer
		{
		public:
			virtual void CopyFromSystemMemory(void* bufferInSystemMemory, int lengthInBytes)
			{
				CopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes);
			}

			virtual void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes) = 0;

			virtual std::shared_ptr<void> CopyToSystemMemory()
			{
				return CopyToSystemMemory(0, GetSizeInBytes());
			}

			virtual std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes) = 0;

			virtual int				GetSizeInBytes() const = 0;
			virtual PixelBufferHint	GetUsageHint() const = 0;
		};

	}
}