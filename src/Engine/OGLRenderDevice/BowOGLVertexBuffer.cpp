#include "BowOGLVertexBuffer.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLVertexBuffer::OGLVertexBuffer(BufferHint usageHint, int sizeInBytes) : m_bufferObject(GL_ARRAY_BUFFER, usageHint, sizeInBytes) { }

		OGLVertexBuffer::~OGLVertexBuffer() { }

		void OGLVertexBuffer::Bind()
		{
			m_bufferObject.Bind();
		}

		void OGLVertexBuffer::UnBind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void OGLVertexBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
		{
			m_bufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
		}

		std::shared_ptr<void> OGLVertexBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
		{
			return m_bufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
		}

		int OGLVertexBuffer::GetSizeInBytes()
		{
			return m_bufferObject.GetSizeInBytes();
		}

		BufferHint OGLVertexBuffer::GetUsageHint()
		{
			return m_bufferObject.GetUsageHint();
		}

	}
}