#include "OGLPixelBuffer.h"
#include "OGLTypeConverter.h"

#include <GL/glew.h>

namespace Bow
{
	extern void ArrayDeleter(void* ptr);

	OGLPixelBuffer::OGLPixelBuffer(GLenum target, BufferHint usageHint, int sizeInBytes) : m_name(new OGLBufferName()), m_sizeInBytes(sizeInBytes), m_target(target), m_usageHint(OGLTypeConverter::To(usageHint))
	{
		FN("OGLPixelBuffer::OGLPixelBuffer()");
		LOG_ASSERT(!(sizeInBytes <= 0), "sizeInBytes must be greater than zero");

		Bind();
        glBufferData(m_target, sizeInBytes, nullptr, m_usageHint);
	}

    void OGLPixelBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		FN("OGLPixelBuffer::CopyFromSystemMemory()");
		LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
        LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");
        LOG_ASSERT(!(lengthInBytes < 0), "lengthInBytes must be greater than or equal to zero.");

        Bind();
		glBufferSubData(m_target, destinationOffsetInBytes, lengthInBytes, bufferInSystemMemory);
	}

    VoidPtr OGLPixelBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		FN("OGLPixelBuffer::CopyToSystemMemory()");
		LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
        LOG_ASSERT(!(sizeInBytes <= 0), "lengthInBytes must be greater than zero.");
        LOG_ASSERT(!(offsetInBytes + sizeInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");

		void* bufferInSystemMemory = malloc(sizeInBytes);

        Bind();
        glGetBufferSubData(m_target, offsetInBytes, sizeInBytes, bufferInSystemMemory);
        return VoidPtr(bufferInSystemMemory, ArrayDeleter);
	}

	int OGLPixelBuffer::GetSizeInBytes() const
	{
		FN("OGLPixelBuffer::GetSizeInBytes()");
		return m_sizeInBytes;
	}

	BufferHint OGLPixelBuffer::GetUsageHint() const
	{
		FN("OGLPixelBuffer::GetUsageHint()");
		return OGLTypeConverter::ToBufferHint(m_usageHint);
	}


	OGLBufferNamePtr OGLPixelBuffer::GetHandle() const
	{
		FN("OGLPixelBuffer::Handle()");
		return m_name;
	}

    void OGLPixelBuffer::Bind() const
	{
		FN("OGLPixelBuffer::Bind()");
		glBindBuffer(m_target, m_name->GetValue());
	}

}

