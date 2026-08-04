#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class MeshBuffers
{
  public:
    MeshBuffers() : m_vertexBufferAttributes(), IndexBuffer(nullptr) {}
    ~MeshBuffers() {}

    VertexBufferAttributeMap GetAttributes() { return m_vertexBufferAttributes; }
    void SetAttribute(uint32_t location, VertexBufferAttributePtr vertexBufferAttribute)
    {
        if (m_vertexBufferAttributes.find(location) == m_vertexBufferAttributes.end())
            m_vertexBufferAttributes[location] = vertexBufferAttribute;
        else
            m_vertexBufferAttributes.insert(std::pair<uint32_t, VertexBufferAttributePtr>(location, vertexBufferAttribute));
    }

    IndexBufferPtr IndexBuffer;

  private:
    VertexBufferAttributeMap m_vertexBufferAttributes;
};

} // namespace bow
