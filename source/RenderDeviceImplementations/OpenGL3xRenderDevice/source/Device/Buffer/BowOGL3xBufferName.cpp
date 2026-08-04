#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xBufferName.h>

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
    LOG_TRACE("glGenBuffers");
    glGenBuffers(1, &m_value);
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
