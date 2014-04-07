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

			void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
			std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

			int	GetSizeInBytes() const;
			PixelBufferHint	GetUsageHint() const;

		private:
			const PixelBufferHint m_usageHint;
			OGLPixelBuffer m_bufferObject;
		};

		typedef std::shared_ptr<OGLReadPixelBuffer> OGLReadPixelBufferPtr;

	}
}