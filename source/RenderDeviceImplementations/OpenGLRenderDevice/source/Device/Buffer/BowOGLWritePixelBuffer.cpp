#include <CoreSystems/BowLogger.h>
#include <OpenGLRenderDevice/Device/Buffer/BowOGLWritePixelBuffer.h>
#include <RenderDevice/Device/Buffer/BowBufferHint.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

BufferHint wpb_bufferHints[] = {BufferHint::StreamRead, BufferHint::StaticRead, BufferHint::DynamicRead};

OGLWritePixelBuffer::OGLWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) : m_UsageHint(usageHint), m_BufferObject(GL_PIXEL_UNPACK_BUFFER, wpb_bufferHints[(int)usageHint], sizeInBytes) { FN("OGLWritePixelBuffer::OGLWritePixelBuffer"); }

void OGLWritePixelBuffer::Bind()
{
    FN("OGLWritePixelBuffer::Bind");

    m_BufferObject.Bind();
}

void OGLWritePixelBuffer::UnBind()
{
    FN("OGLWritePixelBuffer::UnBind");

    LOG_TRACE("glBindBuffer");
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void OGLWritePixelBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
{
    FN("OGLWritePixelBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

std::shared_ptr<void> OGLWritePixelBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
{
    FN("OGLWritePixelBuffer::VCopyToSystemMemory");
    OPTICK_EVENT();

    return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
}

int OGLWritePixelBuffer::VGetSizeInBytes() const
{
    FN("OGLWritePixelBuffer::VGetSizeInBytes");

    return m_BufferObject.GetSizeInBytes();
}

PixelBufferHint OGLWritePixelBuffer::VGetUsageHint() const
{
    FN("OGLWritePixelBuffer::VGetUsageHint");

    return m_UsageHint;
}

} // namespace bow
