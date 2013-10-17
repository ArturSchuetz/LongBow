#include "OGLVertexBuffer.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLVertexBuffer::OGLVertexBuffer(BufferHint usageHint, int sizeInBytes) : m_bufferObject(GL_ARRAY_BUFFER, usageHint, sizeInBytes) { FN("OGLVertexBuffer::OGLVertexBuffer()"); }

    OGLVertexBuffer::~OGLVertexBuffer() { FN("OGLVertexBuffer::~OGLVertexBuffer()"); }

	void OGLVertexBuffer::Bind()
	{
		FN("OGLVertexBuffer::Bind()");
		m_bufferObject.Bind();
	}

    void OGLVertexBuffer::UnBind()
	{
		FN("OGLVertexBuffer::UnBind()");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

    void OGLVertexBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		FN("OGLVertexBuffer::CopyFromSystemMemory()");
        m_bufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}

    VoidPtr OGLVertexBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		FN("OGLVertexBuffer::CopyToSystemMemory()");
		return m_bufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}

	int OGLVertexBuffer::GetSizeInBytes()
	{
		FN("OGLVertexBuffer::GetSizeInBytes()");
		return m_bufferObject.GetSizeInBytes();
	}

    BufferHint OGLVertexBuffer::GetUsageHint()
	{
		FN("OGLVertexBuffer::GetUsageHint()");
		return m_bufferObject.GetUsageHint();
	}

}