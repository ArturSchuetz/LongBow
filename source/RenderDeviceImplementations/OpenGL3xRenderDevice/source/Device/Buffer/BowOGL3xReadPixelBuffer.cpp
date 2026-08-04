#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xReadPixelBuffer.h>
#include <RenderDevice/Device/Buffer/BowBufferHint.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

BufferHint rbp_bufferHints[] = {BufferHint::StreamRead, BufferHint::StaticRead, BufferHint::DynamicRead};

OGLReadPixelBuffer::OGLReadPixelBuffer(PixelBufferHint usageHint, int sizeInBytes) : m_UsageHint(usageHint), m_BufferObject(GL_PIXEL_PACK_BUFFER, rbp_bufferHints[(int)usageHint], sizeInBytes) { FN("OGLReadPixelBuffer::OGLReadPixelBuffer"); }

void OGLReadPixelBuffer::Bind()
{
    FN("OGLReadPixelBuffer::Bind");

    m_BufferObject.Bind();
}

void OGLReadPixelBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
{
    FN("OGLReadPixelBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

std::shared_ptr<void> OGLReadPixelBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
{
    FN("OGLReadPixelBuffer::VCopyToSystemMemory");
    OPTICK_EVENT();

    return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
}

int OGLReadPixelBuffer::VGetSizeInBytes() const
{
    FN("OGLReadPixelBuffer::VGetSizeInBytes");

    return m_BufferObject.GetSizeInBytes();
}

PixelBufferHint OGLReadPixelBuffer::VGetUsageHint() const
{
    FN("OGLReadPixelBuffer::VGetUsageHint");

    return m_UsageHint;
}

} // namespace bow
