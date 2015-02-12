#include "BowOGLBuffer.h"
#include "BowLogger.h"

#include "BowOGLTypeConverter.h"
#include "BowOGLBufferName.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		OGLBuffer::OGLBuffer(unsigned int type, BufferHint usageHint, int sizeInBytes) : m_sizeInBytes(sizeInBytes), m_type(type), m_UsageHint(OGLTypeConverter::To(usageHint))
		{
			m_name = OGLBufferNamePtr(new OGLBufferName());

			//
			// Allocating here with GL.BufferData, then writing with GL.BufferSubData
			// in CopyFromSystemMemory() should not have any serious overhead:
			//
			//   http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=267373#Post267373
			//
			// Alternately, we can delay GL.BufferData until the first
			// CopyFromSystemMemory() call.
			//
			glBindVertexArray(0);
			Bind();
			glBufferData(m_type, sizeInBytes, nullptr, m_UsageHint);
		}

		OGLBuffer::~OGLBuffer() { }

		void OGLBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
		{
			LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
			LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");
			LOG_ASSERT(!(lengthInBytes < 0), "lengthInBytes must be greater than or equal to zero.");

			glBindVertexArray(0);
			Bind();
			glBufferSubData(m_type, destinationOffsetInBytes, lengthInBytes, bufferInSystemMemory);
		}

		std::shared_ptr<void> OGLBuffer::CopyToSystemMemory(int offsetInBytes, int lengthInBytes)
		{
			LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
			LOG_ASSERT(!(lengthInBytes <= 0), "lengthInBytes must be greater than zero.");
			LOG_ASSERT(!(offsetInBytes + lengthInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to SizeInBytes.");

			void* bufferInSystemMemory = malloc(lengthInBytes);

			glBindVertexArray(0);
			Bind();
			glGetBufferSubData(m_type, offsetInBytes, lengthInBytes, bufferInSystemMemory);
			return std::shared_ptr<void>(bufferInSystemMemory, [](void* ptr) { delete[] ptr; });
		}

		int OGLBuffer::GetSizeInBytes()
		{
			return m_sizeInBytes;
		}

		BufferHint OGLBuffer::GetUsageHint()
		{
			return OGLTypeConverter::ToBufferHint(m_UsageHint);
		}

		OGLBufferNamePtr OGLBuffer::GetHandle()
		{
			return m_name;
		}

		void OGLBuffer::Bind()
		{
			glBindBuffer(m_type, m_name->GetValue());
		}

	}
}