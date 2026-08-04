#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderObject.h>

#include <CoreSystems/BowLogger.h>


#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLShaderObject::OGLShaderObject(GLenum shaderType, std::string source)
{
    FN("OGLShaderObject::OGLShaderObject");

    GLint length;

    /* create shader object, set the source, and compile */
    LOG_TRACE("glCreateShader");
    m_shaderObject = glCreateShader(shaderType);
    if (m_shaderObject == 0)
        LOG_ERROR("Could not create Shaderobject.");

    length = strlen(source.c_str());
    const GLchar *shaderSource = source.c_str();
    LOG_TRACE("glShaderSource");
    glShaderSource(m_shaderObject, 1, &shaderSource, &length);
    LOG_TRACE("glCompileShader");
    glCompileShader(m_shaderObject);

    /* make sure the compilation was successful */
    LOG_TRACE("glGetShaderiv");
    glGetShaderiv(m_shaderObject, GL_COMPILE_STATUS, &m_result);
    if (m_result != GL_FALSE)
        LOG_TRACE("Shader successfully loaded!");
    else
        LOG_ERROR(GetCompileLog().c_str());
}

OGLShaderObject::~OGLShaderObject()
{
    FN("OGLShaderObject::~OGLShaderObject");

    LOG_TRACE("glDeleteShader");
    glDeleteShader(m_shaderObject);
}

bool OGLShaderObject::IsReady()
{
    FN("OGLShaderObject::IsReady");

    return m_result != GL_FALSE;
}

uint32_t OGLShaderObject::GetShader()
{
    FN("OGLShaderObject::GetShader");

    return m_shaderObject;
}

std::string OGLShaderObject::GetCompileLog()
{
    FN("OGLShaderObject::GetCompileLog");

    char *buffer = nullptr;
    GLint length, result;

    /* get the shader info log */
    LOG_TRACE("glGetShaderiv");
    glGetShaderiv(m_shaderObject, GL_INFO_LOG_LENGTH, &length);
    buffer = (char *)malloc(length);

    LOG_TRACE("glGetShaderInfoLog");
    glGetShaderInfoLog(m_shaderObject, length, &result, buffer);

    if (result == GL_FALSE)
        LOG_ERROR("Could not get shader info log!");

    if (buffer != nullptr)
        return std::string(buffer);
    else
        return std::string();
}

} // namespace bow
