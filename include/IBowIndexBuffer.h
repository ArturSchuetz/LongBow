#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class IIndexBuffer
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

			virtual int					GetSizeInBytes() = 0;
			virtual BufferHint			GetUsageHint() = 0;
		};

	}
}