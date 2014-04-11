#include "BowOGLIndexBuffer.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLIndexBuffer::OGLIndexBuffer(BufferHint usageHint, int sizeInBytes) : m_bufferObject(GL_ELEMENT_ARRAY_BUFFER, usageHint, sizeInBytes) { }

		OGLIndexBuffer::~OGLIndexBuffer() { }

		void OGLIndexBuffer::Bind()
		{
			m_bufferObject.Bind();
		}

		void OGLIndexBuffer::UnBind()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void OGLIndexBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
		{
			m_bufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
		}

		std::shared_ptr<void> OGLIndexBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
		{
			return m_bufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
		}

		int OGLIndexBuffer::GetSizeInBytes()
		{
			return m_bufferObject.GetSizeInBytes();
		}

		BufferHint OGLIndexBuffer::GetUsageHint()
		{
			return m_bufferObject.GetUsageHint();
		}

	}
}