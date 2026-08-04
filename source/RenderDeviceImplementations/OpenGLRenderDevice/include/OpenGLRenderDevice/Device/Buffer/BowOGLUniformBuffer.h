#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>

#include <OpenGLRenderDevice/Device/Buffer/BowOGLBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Buffer/IBowUniformBuffer.h>

namespace bow
{

class OGLShaderResourceBuffer : public IUniformBuffer
{
  public:
    OGLShaderResourceBuffer(BufferHint usageHint, int64_t sizeInBytes);
    ~OGLShaderResourceBuffer();

    void Bind(uint32_t location);
    static void UnBind();

    void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) override;
    std::shared_ptr<void> VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) override;

    int64_t VGetSizeInBytes();
    BufferHint VGetUsageHint();

  private:
    // You shall not copy
    OGLShaderResourceBuffer(const OGLShaderResourceBuffer &obj) = delete;
    OGLShaderResourceBuffer &operator=(const OGLShaderResourceBuffer &obj) = delete;

    OGLBuffer m_BufferObject;
};

typedef std::shared_ptr<OGLShaderResourceBuffer> OGLShaderResourceBufferPtr;

struct ShaderUniformBufferUnit
{
    ShaderUniformBufferUnit() : Name(""), Binding(0), Size(0), Buffer(nullptr) {}
    ShaderUniformBufferUnit(const std::string &name, uint32_t binding, int size, OGLShaderResourceBufferPtr data) : Name(name), Binding(binding), Size(size), Buffer(data) {}

    std::string Name;
    uint32_t Binding;
    int Size;
    OGLShaderResourceBufferPtr Buffer;
};

} // namespace bow
