#include "BowOGL3xVertexBuffer.h"

#include <GL/glew.h>

namespace bow {

	OGLVertexBuffer::OGLVertexBuffer(BufferHint usageHint, int sizeInBytes) : m_BufferObject(GL_ARRAY_BUFFER, usageHint, sizeInBytes) { }

	OGLVertexBuffer::~OGLVertexBuffer() { }

	void OGLVertexBuffer::Bind()
	{
		m_BufferObject.Bind();
	}

	void OGLVertexBuffer::UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OGLVertexBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}

	std::shared_ptr<void> OGLVertexBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}

	int OGLVertexBuffer::VGetSizeInBytes()
	{
		return m_BufferObject.GetSizeInBytes();
	}

	BufferHint OGLVertexBuffer::VGetUsageHint()
	{
		return m_BufferObject.GetUsageHint();
	}

}
