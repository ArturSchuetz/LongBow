#include <VulkanRenderDevice/Device/Surface/VertexAttributeBindings/BowVulkanVertexBufferAttributes.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanVertexBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanVertexBufferAttributes::VulkanVertexBufferAttributes() : m_count(0), m_maximumArrayIndex(0), m_guid(Utils::GenerateGUID()) { FN("VulkanVertexBufferAttributes::VulkanVertexBufferAttributes"); }

VulkanVertexBufferAttributes::~VulkanVertexBufferAttributes() { FN("VulkanVertexBufferAttributes::~VulkanVertexBufferAttributes"); }

uint64_t VulkanVertexBufferAttributes::GetMaximumArrayIndex()
{
    FN("VulkanVertexBufferAttributes::GetMaximumArrayIndex");

    return m_maximumArrayIndex;
}

VertexBufferAttributeMap VulkanVertexBufferAttributes::GetAttributes()
{
    FN("VulkanVertexBufferAttributes::GetAttributes");

    VertexBufferAttributeMap reslut;
    for (auto it = m_Attributes.begin(); it != m_Attributes.end(); it++)
    {
        reslut.insert(std::pair<uint32_t, VertexBufferAttributePtr>(it->first, it->second));
    }
    return reslut;
}

VertexBufferAttributePtr VulkanVertexBufferAttributes::GetAttribute(int index)
{
    FN("VulkanVertexBufferAttributes::GetAttribute");

    return m_Attributes[index];
}

void VulkanVertexBufferAttributes::SetAttribute(int location, VertexBufferAttributePtr attribute)
{
    FN("VulkanVertexBufferAttributes::SetAttribute");

    if (m_Attributes[location] != attribute)
    {
        if (attribute != nullptr)
        {
            LOG_ASSERT(!(attribute->GetNumberOfComponents() < 1 || attribute->GetNumberOfComponents() > 4), "NumberOfComponents must be between one and four.");

            if (attribute->Normalize())
            {
                LOG_ASSERT(!((attribute->GetComponentDatatype() != ComponentDatatype::Byte) && (attribute->GetComponentDatatype() != ComponentDatatype::UnsignedByte) && (attribute->GetComponentDatatype() != ComponentDatatype::Short) &&
                             (attribute->GetComponentDatatype() != ComponentDatatype::UnsignedShort) && (attribute->GetComponentDatatype() != ComponentDatatype::Int) && (attribute->GetComponentDatatype() != ComponentDatatype::UnsignedInt)),
                           "When Normalize is true, ComponentDatatype must be "
                           "Byte, UnsignedByte, Short, "
                           "UnsignedShort, Int, or UnsignedInt.");
            }

            m_maximumArrayIndex = (((NumberOfVertices(attribute) - 1) > m_maximumArrayIndex) ? (NumberOfVertices(attribute) - 1) : m_maximumArrayIndex);
        }

        if ((m_Attributes[location] != nullptr) && (attribute == nullptr))
        {
            --m_count;
        }
        else if ((m_Attributes[location] == nullptr) && (attribute != nullptr))
        {
            ++m_count;
        }

        m_Attributes[location] = attribute;
    }
}

int VulkanVertexBufferAttributes::NumberOfVertices(VertexBufferAttributePtr attribute)
{
    FN("VulkanVertexBufferAttributes::NumberOfVertices");

    return attribute->GetVertexBuffer()->VGetSizeInBytes() / attribute->GetStrideInBytes();
}

} // namespace bow