#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xIndexBuffer.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLIndexBuffer::OGLIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes) : m_BufferObject(GL_ELEMENT_ARRAY_BUFFER, usageHint, sizeInBytes), m_Datatype(dataType) { FN("OGLIndexBuffer::OGLIndexBuffer"); }

OGLIndexBuffer::~OGLIndexBuffer() { FN("OGLIndexBuffer::~OGLIndexBuffer"); }

void OGLIndexBuffer::Bind()
{
    FN("OGLIndexBuffer::Bind");

    m_BufferObject.Bind();
}

void OGLIndexBuffer::UnBind()
{
    FN("OGLIndexBuffer::UnBind");

    LOG_TRACE("glBindBuffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OGLIndexBuffer::VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("OGLIndexBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

int OGLIndexBuffer::GetCount()
{
    FN("OGLIndexBuffer::GetCount");

    size_t elementsize = m_Datatype == IndexBufferDatatype::UnsignedInt8 ? sizeof(uint8_t) : (m_Datatype == IndexBufferDatatype::UnsignedInt16 ? sizeof(uint16_t) : sizeof(uint32_t));
    return (m_BufferObject.GetSizeInBytes() / elementsize);
}

int64_t OGLIndexBuffer::VGetSizeInBytes()
{
    FN("OGLIndexBuffer::VGetSizeInBytes");

    return m_BufferObject.GetSizeInBytes();
}

BufferHint OGLIndexBuffer::VGetUsageHint()
{
    FN("OGLIndexBuffer::VGetUsageHint");

    return m_BufferObject.GetUsageHint();
}

} // namespace bow
