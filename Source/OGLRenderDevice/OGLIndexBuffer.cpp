#include "OGLIndexBuffer.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLIndexBuffer::OGLIndexBuffer(BufferHint usageHint, int sizeInBytes) : m_bufferObject(GL_ELEMENT_ARRAY_BUFFER, usageHint, sizeInBytes) { FN("OGLIndexBuffer::OGLIndexBuffer()"); }

    OGLIndexBuffer::~OGLIndexBuffer() { FN("OGLIndexBuffer::~OGLIndexBuffer()"); }

	void OGLIndexBuffer::Bind()
	{
		FN("OGLIndexBuffer::Bind()");
		m_bufferObject.Bind();
	}

    void OGLIndexBuffer::UnBind()
	{
		FN("OGLIndexBuffer::UnBind()");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

    void OGLIndexBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		FN("OGLIndexBuffer::CopyFromSystemMemory()");
        m_bufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}

    VoidPtr OGLIndexBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		FN("OGLIndexBuffer::CopyToSystemMemory()");
		return m_bufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}

	int OGLIndexBuffer::GetSizeInBytes()
	{
		FN("OGLIndexBuffer::GetSizeInBytes()");
		return m_bufferObject.GetSizeInBytes();
	}

    BufferHint OGLIndexBuffer::GetUsageHint()
	{
		FN("OGLIndexBuffer::GetUsageHint()");
		return m_bufferObject.GetUsageHint();
	}

}