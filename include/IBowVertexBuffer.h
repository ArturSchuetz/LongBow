#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class IVertexBuffer
		{
		public:
			virtual void VCopyFromSystemMemory(void* bufferInSystemMemory, int lengthInBytes)
			{
				VCopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes);
			}

			virtual void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes) = 0;

			virtual std::shared_ptr<void> VCopyToSystemMemory()
			{
				return VCopyToSystemMemory(0, VGetSizeInBytes());
			}

			virtual std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes) = 0;

			virtual int			VGetSizeInBytes() = 0;
			virtual BufferHint	VGetUsageHint() = 0;
		};

	}
}