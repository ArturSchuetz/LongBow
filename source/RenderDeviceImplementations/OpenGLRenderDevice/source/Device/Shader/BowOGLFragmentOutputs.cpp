#include <OpenGLRenderDevice/Device/Shader/BowOGLFragmentOutputs.h>

#include <CoreSystems/BowLogger.h>


#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLFragmentOutputs::OGLFragmentOutputs(uint32_t program)
{
    FN("OGLFragmentOutputs::OGLFragmentOutputs");

    m_ShaderProgramHandle = program;
}

OGLFragmentOutputs::~OGLFragmentOutputs() { FN("OGLFragmentOutputs::~OGLFragmentOutputs"); }

int OGLFragmentOutputs::operator[](std::string name) const
{
    FN("OGLFragmentOutputs::operator[]");

    uint32_t i = -1;
    LOG_TRACE("glGetFragDataLocation");
    i = glGetFragDataLocation(m_ShaderProgramHandle, name.c_str());

    if (i == -1)
    {
        LOG_ASSERT(i != -1, "Fragment Output Key does not exist.");
        return -1;
    }

    return i;
}

} // namespace bow
