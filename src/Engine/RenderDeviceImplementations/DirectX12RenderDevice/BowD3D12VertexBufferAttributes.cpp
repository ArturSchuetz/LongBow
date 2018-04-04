#include "BowD3D12VertexBufferAttributes.h"
#include "BowLogger.h"

#include "BowVertexBufferAttribute.h"

namespace bow {

	VertexBufferAttributeMap D3DVertexBufferAttributes::GetAttributes()
	{
		return m_Attributes;
	}

	VertexBufferAttributePtr D3DVertexBufferAttributes::GetAttribute(int location)
	{
		return m_Attributes[location];
	}

	void D3DVertexBufferAttributes::SetAttribute(int location, VertexBufferAttributePtr attribute)
	{
		if (m_Attributes[location] != attribute)
		{
			if (attribute != nullptr)
			{
				LOG_ASSERT(!(attribute->GetNumberOfComponents() < 1 || attribute->GetNumberOfComponents() > 4), "NumberOfComponents must be between one and four.");

				if (attribute->Normalize())
				{
					LOG_ASSERT(!(
						(attribute->GetComponentDatatype() != ComponentDatatype::Byte) &&
						(attribute->GetComponentDatatype() != ComponentDatatype::UnsignedByte) &&
						(attribute->GetComponentDatatype() != ComponentDatatype::Short) &&
						(attribute->GetComponentDatatype() != ComponentDatatype::UnsignedShort) &&
						(attribute->GetComponentDatatype() != ComponentDatatype::Int) &&
						(attribute->GetComponentDatatype() != ComponentDatatype::UnsignedInt)
						), "When Normalize is true, ComponentDatatype must be Byte, UnsignedByte, Short, UnsignedShort, Int, or UnsignedInt.");
				}
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

	int D3DVertexBufferAttributes::MaximumArrayIndex()
	{
		int maximumArrayIndex = 0;

		for (auto it = m_Attributes.begin(); it != m_Attributes.end(); it++)
		{
			VertexBufferAttributePtr attribute = m_Attributes[it->first];
			if (attribute != nullptr)
			{
				maximumArrayIndex = std::max(NumberOfVertices(attribute) - 1, maximumArrayIndex);
			}
		}

		return maximumArrayIndex;
	}

	int D3DVertexBufferAttributes::NumberOfVertices(VertexBufferAttributePtr attribute)
	{
		return attribute->GetVertexBuffer()->VGetSizeInBytes() / attribute->GetStrideInBytes();
	}
}
