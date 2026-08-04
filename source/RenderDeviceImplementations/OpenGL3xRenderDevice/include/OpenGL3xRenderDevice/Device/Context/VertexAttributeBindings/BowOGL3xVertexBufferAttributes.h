#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

struct OGLVertexBufferAttribute
{
  public:
    VertexBufferAttributePtr VertexBufferAttribute;
    bool Dirty;
};

typedef std::unordered_map<uint32_t, OGLVertexBufferAttribute> OGLVertexBufferAttributeMap;

class OGLVertexBufferAttributes
{
  public:
    OGLVertexBufferAttributes();
    VertexBufferAttributeMap GetAttributes();

    VertexBufferAttributePtr GetAttribute(int index);
    void SetAttribute(int Location, VertexBufferAttributePtr attribute);

    void Clean(uint32_t vertexArray);
    int GetMaximumArrayIndex();

  private:
    // you shall not copy
    OGLVertexBufferAttributes(const OGLVertexBufferAttributes &) = delete;
    OGLVertexBufferAttributes &operator=(const OGLVertexBufferAttributes &) = delete;

    //! Points one attribute of the given vertex array at its buffer.
    void Attach(uint32_t vertexArray, int index);
    static void Detach(uint32_t vertexArray, int index);

    static inline int NumberOfVertices(VertexBufferAttributePtr attribute);

    OGLVertexBufferAttributeMap m_Attributes;
    int m_count;
    int m_maximumArrayIndex;
    bool m_dirty;
};

} // namespace bow
