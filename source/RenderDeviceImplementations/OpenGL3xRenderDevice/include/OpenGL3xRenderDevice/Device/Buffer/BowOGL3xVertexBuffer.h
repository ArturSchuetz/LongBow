#pragma once
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xBuffer.h>
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowVertexBuffer.h>

namespace bow
{

class OGLVertexBuffer : public IVertexBuffer
{
  public:
    OGLVertexBuffer(BufferHint usageHint, int64_t sizeInBytes);
    ~OGLVertexBuffer();

    void Bind();
    static void UnBind();

    //! The OpenGL name of the underlying buffer object.
    /*!
        Needed by the vertex array, which under direct state access names the
        buffer it reads from instead of relying on it being bound.
    */
    uint32_t GetGLHandle();

    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;

    int64_t VGetSizeInBytes();
    BufferHint VGetUsageHint();

  private:
    // You shall not copy
    OGLVertexBuffer(const OGLVertexBuffer &obj) = delete;
    OGLVertexBuffer &operator=(const OGLVertexBuffer &obj) = delete;

    OGLBuffer m_BufferObject;
};

typedef std::shared_ptr<OGLVertexBuffer> OGLVertexBufferPtr;

} // namespace bow
