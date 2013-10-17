#include "OGLReadPixelBuffer.h"

#include <GL/glew.h>

namespace Bow
{
	BufferHint rbp_bufferHints[] = {
        BufferHint::StreamRead,
        BufferHint::StaticRead,
        BufferHint::DynamicRead
    };

	
	OGLReadPixelBuffer::OGLReadPixelBuffer(PixelBufferHint usageHint, int sizeInBytes) : m_usageHint(usageHint), m_bufferObject( GL_PIXEL_PACK_BUFFER, rbp_bufferHints[(int)usageHint], sizeInBytes )
	{
	}

    void OGLReadPixelBuffer::Bind()
	{
		m_bufferObject.Bind();
	}

    void OGLReadPixelBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		m_bufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}

    VoidPtr OGLReadPixelBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return m_bufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}


    int	OGLReadPixelBuffer::GetSizeInBytes() const
	{
		return m_bufferObject.GetSizeInBytes();
	}

    PixelBufferHint	OGLReadPixelBuffer::GetUsageHint() const
	{
		return m_usageHint;
	}

}