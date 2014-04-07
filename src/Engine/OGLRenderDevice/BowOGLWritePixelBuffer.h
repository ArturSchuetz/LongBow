#pragma once
#include "BowPrerequisites.h"

#include "IBowWritePixelBuffer.h"
#include "BowOGLPixelBuffer.h"

namespace Bow {
	namespace Renderer {

		class OGLWritePixelBuffer : public IWritePixelBuffer
		{
		public:
			OGLWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

			void Bind();
			static void UnBind();

			void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
			std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

			int				GetSizeInBytes() const;
			PixelBufferHint	GetUsageHint() const;

		private:
			const PixelBufferHint m_usageHint;
			OGLPixelBuffer m_bufferObject;
		};

		typedef std::shared_ptr<OGLWritePixelBuffer> OGLWritePixelBufferPtr;

	}
}