#include <OpenGLRenderDevice/Device/Buffer/BowOGLVertexBuffer.h>

#include <OpenGLRenderDevice/Device/Buffer/BowOGLBufferName.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLVertexBuffer::OGLVertexBuffer(BufferHint usageHint, int64_t sizeInBytes) : m_BufferObject(GL_ARRAY_BUFFER, usageHint, sizeInBytes) { FN("OGLVertexBuffer::OGLVertexBuffer"); }

OGLVertexBuffer::~OGLVertexBuffer() { FN("OGLVertexBuffer::Bind"); }

uint32_t OGLVertexBuffer::GetGLHandle()
{
    FN("OGLVertexBuffer::GetGLHandle");

    return m_BufferObject.GetHandle()->GetValue();
}

void OGLVertexBuffer::Bind()
{
    FN("OGLVertexBuffer::Bind");

    m_BufferObject.Bind();
}

void OGLVertexBuffer::UnBind()
{
    FN("OGLVertexBuffer::UnBind");

    LOG_TRACE("glBindBuffer");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OGLVertexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("OGLVertexBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

int64_t OGLVertexBuffer::VGetSizeInBytes()
{
    FN("OGLVertexBuffer::VGetSizeInBytes");

    return m_BufferObject.GetSizeInBytes();
}

BufferHint OGLVertexBuffer::VGetUsageHint()
{
    FN("OGLVertexBuffer::VGetUsageHint");

    return m_BufferObject.GetUsageHint();
}

} // namespace bow
