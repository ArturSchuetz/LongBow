#include "BowOGLVertexBufferAttributes.h"
#include "BowLogger.h"

#include "BowVertexBufferAttribute.h"
#include "BowOGLVertexBuffer.h"
#include "BowOGLTypeConverter.h"

#include <gl\glew.h>

#include <algorithm> // std::max

namespace Bow {
	namespace Renderer {

		VertexBufferAttributeMap OGLVertexBufferAttributes::GetAttributes()
		{
			VertexBufferAttributeMap reslut;
			for (auto it = m_attributes.begin(); it != m_attributes.end(); it++)
			{
				reslut.at(it->first) = it->second.VertexBufferAttribute;
			}
			return reslut;
		}

		VertexBufferAttributePtr OGLVertexBufferAttributes::GetAttribute(int index)
		{
			return m_attributes[index].VertexBufferAttribute;
		}

		int OGLVertexBufferAttributes::NumberOfVertices(VertexBufferAttributePtr attribute)
		{
			return attribute->GetVertexBuffer()->GetSizeInBytes() / attribute->GetStrideInBytes();
		}

		void OGLVertexBufferAttributes::SetAttribute(int location, VertexBufferAttributePtr value)
		{
			if (m_attributes[location].VertexBufferAttribute != value)
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

				if ((m_attributes[location].VertexBufferAttribute != nullptr) && (value == nullptr))
				{
					--m_count;
				}
				else if ((m_attributes[location].VertexBufferAttribute == nullptr) && (value != nullptr))
				{
					++m_count;
				}

				m_attributes[location].VertexBufferAttribute = value;
				m_attributes[location].Dirty = true;
				m_dirty = true;
			}
		}

		void OGLVertexBufferAttributes::Clean()
		{
			if (m_dirty)
			{
				int maximumArrayIndex = 0;

				for (auto it = m_attributes.begin(); it != m_attributes.end(); it++)
				{
					VertexBufferAttributePtr attribute = m_attributes[it->first].VertexBufferAttribute;

					if (m_attributes[it->first].Dirty)
					{
						if (attribute != nullptr)
						{
							Attach(it->first);
						}
						else
						{
							Detach(it->first);
						}

						m_attributes[it->first].Dirty = false;
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

			VertexBufferAttributePtr attribute = m_attributes[location].VertexBufferAttribute;
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
}