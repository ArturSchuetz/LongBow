#include <VulkanRenderDevice/Device/Surface/VertexAttributeBindings/BowVulkanVertexAttributeBindings.h>

#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanIndexBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanVertexBuffer.h>

#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

#include <optick.h>

namespace bow
{

VulkanVertexAttributeBindings::VulkanVertexAttributeBindings() : m_Attributes(), m_indexBuffer(nullptr), m_guid(Utils::GenerateGUID()) { FN("VulkanVertexAttributeBindings::VulkanVertexAttributeBindings"); }

VulkanVertexAttributeBindings::~VulkanVertexAttributeBindings() { FN("VulkanVertexAttributeBindings::~VulkanVertexAttributeBindings"); }

void VulkanVertexAttributeBindings::Bind(VkCommandBuffer commandBuffer)
{
    FN("VulkanVertexAttributeBindings::Bind");

    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkDeviceSize> offsets;

    for (auto &attribute : m_Attributes.GetAttributes())
    {
        VulkanVertexBufferPtr vulkanVertexBuffer = std::dynamic_pointer_cast<VulkanVertexBuffer>(attribute.second->GetVertexBuffer());
        VkBuffer bufferHandle = vulkanVertexBuffer->GetHandle();
        vertexBuffers.push_back(bufferHandle);
        offsets.push_back(0);
    }

    LOG_TRACE("vkCmdBindVertexBuffers %s", m_guid.c_str());
    vkCmdBindVertexBuffers(commandBuffer, 0U, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(), offsets.data());

    if (m_indexBuffer != nullptr)
    {
        m_indexBuffer->Bind(commandBuffer);
    }
}

uint64_t VulkanVertexAttributeBindings::MaximumArrayIndex()
{
    FN("VulkanVertexAttributeBindings::MaximumArrayIndex");

    return m_Attributes.GetMaximumArrayIndex();
}

uint64_t VulkanVertexAttributeBindings::GetIndexCount()
{
    FN("VulkanVertexAttributeBindings::GetIndexCount");

    return m_Attributes.GetMaximumArrayIndex() + 1;
}

uint64_t VulkanVertexAttributeBindings::GetVertexCount()
{
    FN("VulkanVertexAttributeBindings::GetVertexCount");

    VertexBufferAttributeMap attributesMap = m_Attributes.GetAttributes();
    return attributesMap.begin()->second->GetVertexBuffer()->VGetSizeInBytes() / attributesMap.begin()->second->GetStrideInBytes();
}

VertexBufferAttributeMap VulkanVertexAttributeBindings::VGetAttributes()
{
    FN("VulkanVertexAttributeBindings::VGetAttributes");

    return m_Attributes.GetAttributes();
}

void VulkanVertexAttributeBindings::VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer)
{
    FN("VulkanVertexAttributeBindings::VSetAttribute");

    LOG_ASSERT(pointer != nullptr, "Vertex Buffer Attribute not found!");
    m_Attributes.SetAttribute(location, pointer);
}

void VulkanVertexAttributeBindings::VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer)
{
    FN("VulkanVertexAttributeBindings::VSetAttribute");

    LOG_ASSERT(vertexAttribute != nullptr, "Shader Vertex Attribute not found!");
    m_Attributes.SetAttribute(vertexAttribute->Location, pointer);
}

IndexBufferPtr VulkanVertexAttributeBindings::VGetIndexBuffer()
{
    FN("VulkanVertexAttributeBindings::VGetIndexBuffer");

    return std::dynamic_pointer_cast<IIndexBuffer>(m_indexBuffer);
}

void VulkanVertexAttributeBindings::VSetIndexBuffer(IndexBufferPtr pointer)
{
    FN("VulkanVertexAttributeBindings::VSetIndexBuffer");

    m_indexBuffer = std::dynamic_pointer_cast<VulkanIndexBuffer>(pointer);
}

} // namespace bow