#include <OpenGLRenderDevice/Device/Buffer/BowOGLStorageBuffer.h>

#include <OpenGLRenderDevice/Device/Buffer/BowOGLBufferName.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLStorageBuffer::OGLStorageBuffer(BufferHint usageHint, int64_t sizeInBytes) : m_BufferObject(GL_SHADER_STORAGE_BUFFER, usageHint, sizeInBytes) { FN("OGLStorageBuffer::OGLStorageBuffer"); }

OGLStorageBuffer::~OGLStorageBuffer() { FN("OGLStorageBuffer::~OGLStorageBuffer"); }

void OGLStorageBuffer::Bind(uint32_t location)
{
    FN("OGLStorageBuffer::Bind");

    LOG_TRACE("glBindBufferBase");
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, m_BufferObject.GetHandle()->GetValue());
}

void OGLStorageBuffer::UnBind()
{
    FN("OGLStorageBuffer::UnBind");

    LOG_TRACE("glBindBuffer");
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}

void OGLStorageBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("OGLStorageBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

std::shared_ptr<void> OGLStorageBuffer::VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes)
{
    FN("OGLStorageBuffer::VCopyToSystemMemory");
    OPTICK_EVENT();

    return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
}

int64_t OGLStorageBuffer::VGetSizeInBytes()
{
    FN("OGLStorageBuffer::VGetSizeInBytes");

    return m_BufferObject.GetSizeInBytes();
}

BufferHint OGLStorageBuffer::VGetUsageHint()
{
    FN("OGLStorageBuffer::VGetUsageHint");

    return m_BufferObject.GetUsageHint();
}

} // namespace bow
