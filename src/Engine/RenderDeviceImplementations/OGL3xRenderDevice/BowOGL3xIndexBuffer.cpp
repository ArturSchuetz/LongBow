#include "BowOGL3xIndexBuffer.h"

#include <GL/glew.h>

namespace bow {

	OGLIndexBuffer::OGLIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes) : m_BufferObject(GL_ELEMENT_ARRAY_BUFFER, usageHint, sizeInBytes), m_Datatype(dataType) {}

	OGLIndexBuffer::~OGLIndexBuffer() { }

	void OGLIndexBuffer::Bind()
	{
		m_BufferObject.Bind();
	}

	void OGLIndexBuffer::UnBind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OGLIndexBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}

	std::shared_ptr<void> OGLIndexBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}

	int OGLIndexBuffer::GetCount()
	{
		size_t elementsize = m_Datatype == IndexBufferDatatype::UnsignedInt ? sizeof(unsigned int) : sizeof(unsigned short);
		return (m_BufferObject.GetSizeInBytes() / elementsize);
	}

	int OGLIndexBuffer::VGetSizeInBytes()
	{
		return m_BufferObject.GetSizeInBytes();
	}

	BufferHint OGLIndexBuffer::VGetUsageHint()
	{
		return m_BufferObject.GetUsageHint();
	}

}
