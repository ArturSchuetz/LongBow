#pragma once
#include <OpenGLRenderDevice/Device/Buffer/BowOGLBuffer.h>
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/BowIndexBufferDatatype.h>
#include <RenderDevice/Device/Buffer/IBowIndexBuffer.h>

namespace bow
{

class OGLIndexBuffer : public IIndexBuffer
{
  public:
    OGLIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int64_t sizeInBytes);
    ~OGLIndexBuffer();

    void Bind();
    static void UnBind();
    int GetCount();

    void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes);

    int64_t VGetSizeInBytes();
    BufferHint VGetUsageHint();
    IndexBufferDatatype GetDatatype() { return m_Datatype; }

  private:
    // You shall not copy
    OGLIndexBuffer(const OGLIndexBuffer &obj) = delete;
    OGLIndexBuffer &operator=(const OGLIndexBuffer &obj) = delete;

    OGLBuffer m_BufferObject;
    IndexBufferDatatype m_Datatype;
};

typedef std::shared_ptr<OGLIndexBuffer> OGLIndexBufferPtr;

} // namespace bow
