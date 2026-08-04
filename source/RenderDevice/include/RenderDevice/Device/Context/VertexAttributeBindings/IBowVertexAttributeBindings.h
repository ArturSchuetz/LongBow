#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IVertexAttributeBindings
{
  public:
    virtual ~IVertexAttributeBindings() {}

    virtual VertexBufferAttributeMap VGetAttributes() = 0;
    virtual void VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer) = 0;
    virtual void VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer) = 0;

    virtual IndexBufferPtr VGetIndexBuffer() = 0;
    virtual void VSetIndexBuffer(IndexBufferPtr pointer) = 0;
};

} // namespace bow
