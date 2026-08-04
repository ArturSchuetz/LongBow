#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowStorageBuffer.h>

namespace bow
{

class OGLStorageBuffer : public IStorageBuffer
{
  public:
    OGLStorageBuffer(BufferHint usageHint, int64_t sizeInBytes);
    ~OGLStorageBuffer();

    void Bind(uint32_t location);
    static void UnBind();

    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;
    std::shared_ptr<void> VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) override;

    int64_t VGetSizeInBytes();
    BufferHint VGetUsageHint();

  private:
    // You shall not copy
    OGLStorageBuffer(const OGLStorageBuffer &obj) = delete;
    OGLStorageBuffer &operator=(const OGLStorageBuffer &obj) = delete;

    OGLBuffer m_BufferObject;
};

typedef std::shared_ptr<OGLStorageBuffer> OGLStorageBufferPtr;

struct ShaderStorageBufferUnit
{
    ShaderStorageBufferUnit() : Name(""), Binding(0), Size(0), Buffer(nullptr) {}
    ShaderStorageBufferUnit(const std::string &name, uint32_t binding, int size, OGLStorageBufferPtr data) : Name(name), Binding(binding), Size(size), Buffer(data) {}

    std::string Name;
    uint32_t Binding;
    int Size;
    OGLStorageBufferPtr Buffer;
};

} // namespace bow
