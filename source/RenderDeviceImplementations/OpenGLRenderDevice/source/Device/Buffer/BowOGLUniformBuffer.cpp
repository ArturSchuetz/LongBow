#include <OpenGLRenderDevice/Device/Buffer/BowOGLUniformBuffer.h>

#include <OpenGLRenderDevice/Device/Buffer/BowOGLBufferName.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLShaderResourceBuffer::OGLShaderResourceBuffer(BufferHint usageHint, int64_t sizeInBytes) : m_BufferObject(GL_UNIFORM_BUFFER, usageHint, sizeInBytes) { FN("OGLShaderResourceBuffer::OGLShaderResourceBuffer"); }

OGLShaderResourceBuffer::~OGLShaderResourceBuffer() { FN("OGLShaderResourceBuffer::~OGLShaderResourceBuffer"); }

void OGLShaderResourceBuffer::Bind(uint32_t location)
{
    FN("OGLShaderResourceBuffer::Bind");

    LOG_TRACE("glBindBufferBase");
    glBindBufferBase(GL_UNIFORM_BUFFER, location, m_BufferObject.GetHandle()->GetValue());
}

void OGLShaderResourceBuffer::UnBind()
{
    FN("OGLShaderResourceBuffer::UnBind");

    LOG_TRACE("glBindBuffer");
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
}

void OGLShaderResourceBuffer::VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes)
{
    FN("OGLShaderResourceBuffer::VCopyFromSystemMemory");
    OPTICK_EVENT();

    m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
}

std::shared_ptr<void> OGLShaderResourceBuffer::VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes)
{
    FN("OGLShaderResourceBuffer::VCopyToSystemMemory");
    OPTICK_EVENT();

    return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
}

int64_t OGLShaderResourceBuffer::VGetSizeInBytes()
{
    FN("OGLShaderResourceBuffer::VGetSizeInBytes");

    return m_BufferObject.GetSizeInBytes();
}

BufferHint OGLShaderResourceBuffer::VGetUsageHint()
{
    FN("OGLShaderResourceBuffer::VGetUsageHint");

    return m_BufferObject.GetUsageHint();
}

} // namespace bow
