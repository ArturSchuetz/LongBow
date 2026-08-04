#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xBufferName.h>

#include <CoreSystems/BowLogger.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLBuffer::OGLBuffer(uint32_t type, BufferHint usageHint, int64_t sizeInBytes) : m_sizeInBytes(sizeInBytes), m_type(type), m_UsageHint(OGLTypeConverter::To(usageHint))
{
    FN("OGLBuffer::OGLBuffer");

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
    LOG_TRACE("glBindVertexArray");
    glBindVertexArray(0);
    Bind();
    LOG_TRACE("glBufferData");
    glBufferData(m_type, sizeInBytes, nullptr, m_UsageHint);
}

OGLBuffer::~OGLBuffer() { FN("OGLBuffer::~OGLBuffer"); }

void OGLBuffer::CopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("OGLBuffer::CopyFromSystemMemory");

    LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or "
                                                                            "equal to SizeInBytes.");
    LOG_ASSERT(!(lengthInBytes < 0), "lengthInBytes must be greater than or equal to zero.");

    LOG_TRACE("glBindVertexArray");
    glBindVertexArray(0);
    Bind();
    LOG_TRACE("glBufferSubData");
    glBufferSubData(m_type, destinationOffsetInBytes, lengthInBytes, bufferInSystemMemory);
}

std::shared_ptr<void> OGLBuffer::CopyToSystemMemory(int64_t offsetInBytes, int64_t lengthInBytes)
{
    FN("OGLBuffer::CopyToSystemMemory");

    LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(lengthInBytes <= 0), "lengthInBytes must be greater than zero.");
    LOG_ASSERT(!(offsetInBytes + lengthInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to "
                                                                 "SizeInBytes.");

    void *bufferInSystemMemory = malloc(lengthInBytes);

    LOG_TRACE("glBindVertexArray");
    glBindVertexArray(0);
    Bind();
    LOG_TRACE("glGetBufferSubData");
    glGetBufferSubData(m_type, offsetInBytes, lengthInBytes, bufferInSystemMemory);
    return std::shared_ptr<void>(bufferInSystemMemory, [](void *ptr) { delete[] ptr; });
}

int64_t OGLBuffer::GetSizeInBytes()
{
    FN("OGLBuffer::GetSizeInBytes");

    return m_sizeInBytes;
}

BufferHint OGLBuffer::GetUsageHint()
{
    FN("OGLBuffer::GetUsageHint");

    return OGLTypeConverter::ToBufferHint(m_UsageHint);
}

OGLBufferNamePtr OGLBuffer::GetHandle()
{
    FN("OGLBuffer::GetHandle");

    return m_name;
}

void OGLBuffer::Bind()
{
    FN("OGLBuffer::Bind");

    LOG_TRACE("glBindBuffer");
    glBindBuffer(m_type, m_name->GetValue());
}

} // namespace bow
