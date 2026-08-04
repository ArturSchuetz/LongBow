
#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xBufferName.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xPixelBuffer.h>

#include <CoreSystems/BowLogger.h>


#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLPixelBuffer::OGLPixelBuffer(GLenum target, BufferHint usageHint, int sizeInBytes) : m_name(new OGLBufferName()), m_sizeInBytes(sizeInBytes), m_target(target), m_UsageHint(OGLTypeConverter::To(usageHint))
{
    FN("OGLPixelBuffer::OGLPixelBuffer");

    LOG_ASSERT(!(sizeInBytes <= 0), "sizeInBytes must be greater than zero");

    Bind();
    LOG_TRACE("glBufferData");
    glBufferData(m_target, sizeInBytes, nullptr, m_UsageHint);
}

void OGLPixelBuffer::CopyFromSystemMemory(void *bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
{
    FN("OGLPixelBuffer::CopyFromSystemMemory");

    LOG_ASSERT(!(destinationOffsetInBytes < 0), "destinationOffsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(destinationOffsetInBytes + lengthInBytes > m_sizeInBytes), "destinationOffsetInBytes + lengthInBytes must be less than or "
                                                                            "equal to SizeInBytes.");
    LOG_ASSERT(!(lengthInBytes < 0), "lengthInBytes must be greater than or equal to zero.");

    Bind();
    LOG_TRACE("glBufferSubData");
    glBufferSubData(m_target, destinationOffsetInBytes, lengthInBytes, bufferInSystemMemory);
}

std::shared_ptr<void> OGLPixelBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
{
    FN("OGLPixelBuffer::CopyToSystemMemory");

    LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
    LOG_ASSERT(!(sizeInBytes <= 0), "lengthInBytes must be greater than zero.");
    LOG_ASSERT(!(offsetInBytes + sizeInBytes > m_sizeInBytes), "offsetInBytes + lengthInBytes must be less than or equal to "
                                                               "SizeInBytes.");

    void *bufferInSystemMemory = malloc(sizeInBytes);

    Bind();
    LOG_TRACE("glGetBufferSubData");
    glGetBufferSubData(m_target, offsetInBytes, sizeInBytes, bufferInSystemMemory);
    return std::shared_ptr<void>(bufferInSystemMemory, [](void *ptr) { delete[] ptr; });
}

int OGLPixelBuffer::GetSizeInBytes() const
{
    FN("OGLPixelBuffer::GetSizeInBytes");

    return m_sizeInBytes;
}

BufferHint OGLPixelBuffer::GetUsageHint() const
{
    FN("OGLPixelBuffer::GetUsageHint");

    return OGLTypeConverter::ToBufferHint(m_UsageHint);
}

OGLBufferNamePtr OGLPixelBuffer::GetHandle() const
{
    FN("OGLPixelBuffer::GetHandle");

    return m_name;
}

void OGLPixelBuffer::Bind() const
{
    FN("OGLPixelBuffer::Bind");

    LOG_TRACE("glBindBuffer");
    glBindBuffer(m_target, m_name->GetValue());
}

} // namespace bow
