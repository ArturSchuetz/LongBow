#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

class OGLBufferName
{
  public:
    OGLBufferName();
    ~OGLBufferName();

    uint32_t GetValue();

  private:
    // You shall not copy
    OGLBufferName(const OGLBufferName &obj) = delete;
    OGLBufferName &operator=(const OGLBufferName &obj) = delete;

    uint32_t m_value;
};

typedef std::shared_ptr<OGLBufferName> OGLBufferNamePtr;

} // namespace bow
