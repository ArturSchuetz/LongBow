#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

typedef std::shared_ptr<class OGLBufferName> OGLBufferNamePtr;

class OGLBuffer
{
  public:
    OGLBuffer(uint32_t type, BufferHint usageHint, int64_t sizeInBytes);
    ~OGLBuffer();

    void CopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes);
    std::shared_ptr<void> CopyToSystemMemory(int64_t offsetInBytes, int64_t lengthInBytes);

    int64_t GetSizeInBytes();
    BufferHint GetUsageHint();
    OGLBufferNamePtr GetHandle();

    void Bind();

  private:
    // You shall not copy
    OGLBuffer(const OGLBuffer &obj) = delete;
    OGLBuffer &operator=(const OGLBuffer &obj) = delete;

    OGLBufferNamePtr m_name;
    const int64_t m_sizeInBytes;
    const uint32_t m_type;
    const uint32_t m_UsageHint;
};

typedef std::shared_ptr<OGLBuffer> OGLBufferPtr;

} // namespace bow
