#include "BowOGL3xWritePixelBuffer.h"
#include "BowLogger.h"

#include "BowBufferHint.h"

#include <GL/glew.h>

namespace bow {

	BufferHint wpb_bufferHints[] = {
		BufferHint::StreamRead,
		BufferHint::StaticRead,
		BufferHint::DynamicRead
	};


	OGLWritePixelBuffer::OGLWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) : m_UsageHint(usageHint), m_BufferObject(GL_PIXEL_UNPACK_BUFFER, wpb_bufferHints[(int)usageHint], sizeInBytes)
	{
	}


	void OGLWritePixelBuffer::Bind()
	{
		m_BufferObject.Bind();
	}


	void OGLWritePixelBuffer::UnBind()
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}


	void OGLWritePixelBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}


	std::shared_ptr<void> OGLWritePixelBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}


	int	OGLWritePixelBuffer::VGetSizeInBytes() const
	{
		return m_BufferObject.GetSizeInBytes();
	}


	PixelBufferHint	OGLWritePixelBuffer::VGetUsageHint() const
	{
		return m_UsageHint;
	}

}
