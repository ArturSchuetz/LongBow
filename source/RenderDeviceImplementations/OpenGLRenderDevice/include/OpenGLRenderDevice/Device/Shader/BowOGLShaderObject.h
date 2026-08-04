#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

typedef uint32_t GLenum;

class OGLShaderObject
{
  public:
    OGLShaderObject(GLenum shaderType, std::string source);
    ~OGLShaderObject();

    std::string GetCompileLog();
    uint32_t GetShader();
    bool IsReady();

  private:
    // You shall not copy
    OGLShaderObject(const OGLShaderObject &obj) = delete;
    OGLShaderObject &operator=(const OGLShaderObject &obj) = delete;

    uint32_t m_shaderObject;
    int m_result;
};

typedef std::shared_ptr<OGLShaderObject> OGLShaderObjectPtr;

} // namespace bow
