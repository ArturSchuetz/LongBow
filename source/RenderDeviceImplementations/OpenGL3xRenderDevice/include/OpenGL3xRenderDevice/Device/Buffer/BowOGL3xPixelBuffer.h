#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

typedef std::shared_ptr<class OGLBufferName> OGLBufferNamePtr;
typedef uint32_t GLenum;

class OGLPixelBuffer
{
  public:
    OGLPixelBuffer(GLenum target, BufferHint usageHint, int sizeInBytes);

    void CopyFromSystemMemory(void *bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
    // void CopyFromBitmap(Bitmap bitmap)

    std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes);
    // Bitmap CopyToBitmap(int width, int height, ImagingPixelFormat
    // pixelFormat);

    int GetSizeInBytes() const;
    BufferHint GetUsageHint() const;

    OGLBufferNamePtr GetHandle() const;
    void Bind() const;

  private:
    // You shall not copy
    OGLPixelBuffer(const OGLPixelBuffer &obj) = delete;
    OGLPixelBuffer &operator=(const OGLPixelBuffer &obj) = delete;

    OGLBufferNamePtr m_name;
    const int m_sizeInBytes;
    const GLenum m_target;
    const GLenum m_UsageHint;
};

typedef std::shared_ptr<OGLPixelBuffer> OGLPixelBufferPtr;
} // namespace bow
