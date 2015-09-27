#include "BowOGLPixelBuffer.h"
#include "BowLogger.h"

#include "BowOGLTypeConverter.h"
#include "BowOGLBufferName.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLPixelBuffer::OGLPixelBuffer(GLenum target, BufferHint usageHint, int sizeInBytes) : m_name(new OGLBufferName()), m_sizeInBytes(sizeInBytes), m_target(target), m_UsageHint(OGLTypeConverter::To(usageHint))
		{
			LOG_ASSERT(!(sizeInBytes <= 0), "sizeInBytes must be greater than zero");

			Bind();
			glBufferData(m_target, sizeInBytes, nullptr, m_UsageHint);
		}


		void OGLPixelBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
		{
			LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
			LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");
			LOG_ASSERT(!(lengthInBytes < 0), "lengthInBytes must be greater than or equal to zero.");

			Bind();
			glBufferSubData(m_target, destinationOffsetInBytes, lengthInBytes, bufferInSystemMemory);
		}


		std::shared_ptr<void> OGLPixelBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
		{
			LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
			LOG_ASSERT(!(sizeInBytes <= 0), "lengthInBytes must be greater than zero.");
			LOG_ASSERT(!(offsetInBytes + sizeInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");

			void* bufferInSystemMemory = malloc(sizeInBytes);

			Bind();
			glGetBufferSubData(m_target, offsetInBytes, sizeInBytes, bufferInSystemMemory);
			return std::shared_ptr<void>(bufferInSystemMemory, [](void* ptr) { delete[] ptr; });
		}


		int OGLPixelBuffer::GetSizeInBytes() const
		{
			return m_sizeInBytes;
		}


		BufferHint OGLPixelBuffer::GetUsageHint() const
		{
			return OGLTypeConverter::ToBufferHint(m_UsageHint);
		}


		OGLBufferNamePtr OGLPixelBuffer::GetHandle() const
		{
			return m_name;
		}


		void OGLPixelBuffer::Bind() const
		{
			glBindBuffer(m_target, m_name->GetValue());
		}

	}
}

