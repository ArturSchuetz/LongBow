#pragma once
#include <OpenGLRenderDevice/Device/Buffer/BowOGLPixelBuffer.h>
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowReadPixelBuffer.h>

namespace bow
{

class OGLReadPixelBuffer : public IReadPixelBuffer
{
  public:
    OGLReadPixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

    void Bind();

    void VCopyFromSystemMemory(void *bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
    std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

    int VGetSizeInBytes() const;
    PixelBufferHint VGetUsageHint() const;

  private:
    // You shall not copy
    OGLReadPixelBuffer(const OGLReadPixelBuffer &obj) = delete;
    OGLReadPixelBuffer &operator=(const OGLReadPixelBuffer &obj) = delete;

    const PixelBufferHint m_UsageHint;
    OGLPixelBuffer m_BufferObject;
};

typedef std::shared_ptr<OGLReadPixelBuffer> OGLReadPixelBufferPtr;

} // namespace bow
