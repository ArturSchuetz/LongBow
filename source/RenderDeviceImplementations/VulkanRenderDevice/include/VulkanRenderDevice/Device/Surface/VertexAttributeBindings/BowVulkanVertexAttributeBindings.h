#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/Device/Surface/VertexAttributeBindings/BowVulkanVertexBufferAttributes.h>
#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/IBowVertexAttributeBindings.h>

namespace bow
{

class VulkanVertexAttributeBindings : public IVertexAttributeBindings
{
  public:
    VulkanVertexAttributeBindings();
    ~VulkanVertexAttributeBindings();

    void Bind(VkCommandBuffer commandBuffer);
    uint64_t MaximumArrayIndex();

    uint64_t GetIndexCount();
    uint64_t GetVertexCount();

    VertexBufferAttributeMap VGetAttributes() override;
    void VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer) override;
    void VSetAttribute(ShaderVertexAttributePtr Location, VertexBufferAttributePtr pointer) override;

    IndexBufferPtr VGetIndexBuffer() override;
    void VSetIndexBuffer(IndexBufferPtr pointer) override;

  private:
    // you shall not copy!
    VulkanVertexAttributeBindings(const VulkanVertexAttributeBindings &) = delete;
    VulkanVertexAttributeBindings &operator=(const VulkanVertexAttributeBindings &) = delete;

    VulkanVertexBufferAttributes m_Attributes;
    VulkanIndexBufferPtr m_indexBuffer;

    std::string m_guid;
};

} // namespace bow