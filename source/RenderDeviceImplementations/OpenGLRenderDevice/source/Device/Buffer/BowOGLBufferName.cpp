#include <OpenGLRenderDevice/Device/Buffer/BowOGLBufferName.h>

#include <CoreSystems/BowLogger.h>


#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLBufferName::OGLBufferName()
{
    FN("OGLBufferName::OGLBufferName");

    m_value = 0;

    // glCreateBuffers returns a name that already refers to a buffer object.
    // glGenBuffers only reserves the name; the object does not exist until the
    // name is bound once, which is why the old code had to bind before it
    // could do anything. Falls back when the driver is below 4.5.
    if (glCreateBuffers != nullptr)
    {
        LOG_TRACE("glCreateBuffers");
        glCreateBuffers(1, &m_value);
    }
    else
    {
        LOG_TRACE("glGenBuffers");
        glGenBuffers(1, &m_value);
    }
}

OGLBufferName::~OGLBufferName()
{
    FN("OGLBufferName::~OGLBufferName");

    if (m_value != 0)
    {
        LOG_TRACE("glDeleteBuffers");
        glDeleteBuffers(1, &m_value);
        m_value = 0;
    }
}

uint32_t OGLBufferName::GetValue()
{
    FN("OGLBufferName::GetValue");

    return m_value;
}

} // namespace bow
