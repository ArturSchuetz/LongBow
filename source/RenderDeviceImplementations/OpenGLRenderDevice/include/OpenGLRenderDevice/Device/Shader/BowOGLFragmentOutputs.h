#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>

#include <RenderDevice/Device/Shader/IBowFragmentOutputs.h>

namespace bow
{

class OGLFragmentOutputs : public IFragmentOutputs
{
  public:
    OGLFragmentOutputs(uint32_t program);
    ~OGLFragmentOutputs();

    int operator[](std::string name) const;

  private:
    // you shall not copy
    OGLFragmentOutputs(const OGLFragmentOutputs &) = delete;
    OGLFragmentOutputs &operator=(const OGLFragmentOutputs &) = delete;

    uint32_t m_ShaderProgramHandle;
};

typedef std::shared_ptr<OGLFragmentOutputs> OGLFragmentOutputsPtr;

} // namespace bow
