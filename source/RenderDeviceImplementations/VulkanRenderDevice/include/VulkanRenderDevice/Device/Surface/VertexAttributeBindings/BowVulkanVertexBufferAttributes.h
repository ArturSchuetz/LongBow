#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

typedef std::unordered_map<uint32_t, VertexBufferAttributePtr> VulkanVertexBufferAttributeMap;

class VulkanVertexBufferAttributes
{
  public:
    VulkanVertexBufferAttributes();
    ~VulkanVertexBufferAttributes();

    uint64_t GetMaximumArrayIndex();
    VertexBufferAttributeMap GetAttributes();
    VertexBufferAttributePtr GetAttribute(int index);

    void SetAttribute(int location, VertexBufferAttributePtr attribute);

    static inline int NumberOfVertices(VertexBufferAttributePtr attribute);

  private:
    // you shall not copy!
    VulkanVertexBufferAttributes(const VulkanVertexBufferAttributes &) = delete;
    VulkanVertexBufferAttributes &operator=(const VulkanVertexBufferAttributes &) = delete;

    VulkanVertexBufferAttributeMap m_Attributes;
    int m_count;
    int m_maximumArrayIndex;

    std::string m_guid;
};

} // namespace bow