#include "BowOGLWritePixelBuffer.h"
#include "BowLogger.h"

#include "BowBufferHint.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		BufferHint wpb_bufferHints[] = {
			BufferHint::StreamRead,
			BufferHint::StaticRead,
			BufferHint::DynamicRead
		};


		OGLWritePixelBuffer::OGLWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) : m_usageHint(usageHint), m_bufferObject(GL_PIXEL_UNPACK_BUFFER, wpb_bufferHints[(int)usageHint], sizeInBytes)
		{
		}


		void OGLWritePixelBuffer::Bind()
		{
			m_bufferObject.Bind();
		}


		void OGLWritePixelBuffer::UnBind()
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}


		void OGLWritePixelBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
		{
			m_bufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
		}


		std::shared_ptr<void> OGLWritePixelBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
		{
			return m_bufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
		}


		int	OGLWritePixelBuffer::GetSizeInBytes() const
		{
			return m_bufferObject.GetSizeInBytes();
		}


		PixelBufferHint	OGLWritePixelBuffer::GetUsageHint() const
		{
			return m_usageHint;
		}

	}
}