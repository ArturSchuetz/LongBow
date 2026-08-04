#pragma once
#include <OpenGL3xRenderDevice/Device/Context/VertexAttributeBindings/BowOGL3xVertexBufferAttributes.h>
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/IBowVertexAttributeBindings.h>

namespace bow
{

typedef std::shared_ptr<class OGLIndexBuffer> OGLIndexBufferPtr;

class OGLVertexAttributeBindings : public IVertexAttributeBindings
{
  public:
    OGLVertexAttributeBindings();
    ~OGLVertexAttributeBindings();

    void Bind();
    void Clean();

    uint64_t MaximumArrayIndex();

    VertexBufferAttributeMap VGetAttributes();
    void VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer);
    void VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer);

    IndexBufferPtr VGetIndexBuffer();
    void VSetIndexBuffer(IndexBufferPtr pointer);

  private:
    // you shall not copy
    OGLVertexAttributeBindings(const OGLVertexAttributeBindings &) = delete;
    OGLVertexAttributeBindings &operator=(const OGLVertexAttributeBindings &) = delete;

    OGLVertexBufferAttributes m_Attributes;
    OGLIndexBufferPtr m_indexBuffer;
    bool m_dirtyIndexBuffer;

    uint32_t m_VertexArrayHandle;
};

typedef std::shared_ptr<OGLVertexAttributeBindings> OGLVertexAttributeBindingsPtr;

} // namespace bow
