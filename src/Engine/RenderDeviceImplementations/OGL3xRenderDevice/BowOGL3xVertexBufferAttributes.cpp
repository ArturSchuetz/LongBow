#include "BowOGL3xVertexBufferAttributes.h"
#include "BowLogger.h"

#include "BowVertexBufferAttribute.h"
#include "BowOGL3xVertexBuffer.h"
#include "BowOGL3xTypeConverter.h"

#include <gl\glew.h>

#include <algorithm> // std::max

namespace bow {

	VertexBufferAttributeMap OGLVertexBufferAttributes::GetAttributes()
	{
		VertexBufferAttributeMap reslut;
		for (auto it = m_Attributes.begin(); it != m_Attributes.end(); it++)
		{
			reslut.insert(std::pair<unsigned int, VertexBufferAttributePtr>(it->first, it->second.VertexBufferAttribute));
		}
		return reslut;
	}

	VertexBufferAttributePtr OGLVertexBufferAttributes::GetAttribute(int index)
	{
		return m_Attributes[index].VertexBufferAttribute;
	}

	int OGLVertexBufferAttributes::NumberOfVertices(VertexBufferAttributePtr attribute)
	{
		return attribute->GetVertexBuffer()->VGetSizeInBytes() / attribute->GetStrideInBytes();
	}

	void OGLVertexBufferAttributes::SetAttribute(int location, VertexBufferAttributePtr value)
	{
		if (m_Attributes[location].VertexBufferAttribute != value)
		{
			if (value != nullptr)
			{
				LOG_ASSERT(!(value->GetNumberOfComponents() < 1 || value->GetNumberOfComponents() > 4), "NumberOfComponents must be between one and four.");

				if (value->Normalize())
				{
					LOG_ASSERT(!(
						(value->GetComponentDatatype() != ComponentDatatype::Byte) &&
						(value->GetComponentDatatype() != ComponentDatatype::UnsignedByte) &&
						(value->GetComponentDatatype() != ComponentDatatype::Short) &&
						(value->GetComponentDatatype() != ComponentDatatype::UnsignedShort) &&
						(value->GetComponentDatatype() != ComponentDatatype::Int) &&
						(value->GetComponentDatatype() != ComponentDatatype::UnsignedInt)
						), "When Normalize is true, ComponentDatatype must be Byte, UnsignedByte, Short, UnsignedShort, Int, or UnsignedInt.");
				}
			}

			if ((m_Attributes[location].VertexBufferAttribute != nullptr) && (value == nullptr))
			{
				--m_count;
			}
			else if ((m_Attributes[location].VertexBufferAttribute == nullptr) && (value != nullptr))
			{
				++m_count;
			}

			m_Attributes[location].VertexBufferAttribute = value;
			m_Attributes[location].Dirty = true;
			m_dirty = true;
		}
	}

	void OGLVertexBufferAttributes::Clean()
	{
		if (m_dirty)
		{
			int maximumArrayIndex = 0;

			for (auto it = m_Attributes.begin(); it != m_Attributes.end(); it++)
			{
				VertexBufferAttributePtr attribute = m_Attributes[it->first].VertexBufferAttribute;

				if (m_Attributes[it->first].Dirty)
				{
					if (attribute != nullptr)
					{
						Attach(it->first);
					}
					else
					{
						Detach(it->first);
					}

					m_Attributes[it->first].Dirty = false;
				}

				if (attribute != nullptr)
				{
					maximumArrayIndex = std::max(NumberOfVertices(attribute) - 1, maximumArrayIndex);
				}
			}

			m_dirty = false;
			m_maximumArrayIndex = maximumArrayIndex;
		}
	}

	int OGLVertexBufferAttributes::GetMaximumArrayIndex()
	{
		LOG_ASSERT(!m_dirty, "MaximumArrayIndex is not valid until Clean() is called.");
		return m_maximumArrayIndex;
	}

	void OGLVertexBufferAttributes::Attach(int location)
	{
		glEnableVertexAttribArray(location);

		VertexBufferAttributePtr attribute = m_Attributes[location].VertexBufferAttribute;
		OGLVertexBufferPtr bufferObjectGL = std::dynamic_pointer_cast<OGLVertexBuffer>(attribute->GetVertexBuffer());

		bufferObjectGL->Bind();
		glVertexAttribPointer(location,
			attribute->GetNumberOfComponents(),
			OGLTypeConverter::To(attribute->GetComponentDatatype()),
			attribute->Normalize(),
			attribute->GetStrideInBytes(),
			(void*)attribute->GetOffsetInBytes());
	}

	void OGLVertexBufferAttributes::Detach(int location)
	{
		glDisableVertexAttribArray(location);
	}

}
