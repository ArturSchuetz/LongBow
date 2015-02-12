#include "BowOGLReadPixelBuffer.h"

#include "BowBufferHint.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		BufferHint rbp_bufferHints[] = {
			BufferHint::StreamRead,
			BufferHint::StaticRead,
			BufferHint::DynamicRead
		};


		OGLReadPixelBuffer::OGLReadPixelBuffer(PixelBufferHint usageHint, int sizeInBytes) : m_UsageHint(usageHint), m_BufferObject(GL_PIXEL_PACK_BUFFER, rbp_bufferHints[(int)usageHint], sizeInBytes)
		{
		}

		void OGLReadPixelBuffer::Bind()
		{
			m_BufferObject.Bind();
		}

		void OGLReadPixelBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
		{
			m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
		}

		std::shared_ptr<void> OGLReadPixelBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
		{
			return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
		}

		int	OGLReadPixelBuffer::VGetSizeInBytes() const
		{
			return m_BufferObject.GetSizeInBytes();
		}

		PixelBufferHint	OGLReadPixelBuffer::VGetUsageHint() const
		{
			return m_UsageHint;
		}

	}
}