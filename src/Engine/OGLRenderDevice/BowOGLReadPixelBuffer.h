#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "IBowReadPixelBuffer.h"
#include "BowOGLPixelBuffer.h"

namespace Bow {
	namespace Renderer {

		class OGLReadPixelBuffer : public IReadPixelBuffer
		{
		public:
			OGLReadPixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

			void Bind();

			void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
			std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

			int	VGetSizeInBytes() const;
			PixelBufferHint	VGetUsageHint() const;

		private:
			const PixelBufferHint m_UsageHint;
			OGLPixelBuffer m_BufferObject;
		};

		typedef std::shared_ptr<OGLReadPixelBuffer> OGLReadPixelBufferPtr;

	}
}