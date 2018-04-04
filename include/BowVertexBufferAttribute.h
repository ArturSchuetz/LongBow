#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "IBowVertexBuffer.h"

#include "BowComponentDatatype.h"
#include "BowVertexArraySizes.h"

#include <unordered_map>

namespace bow {

	struct VertexBufferAttribute
	{
	public:
		VertexBufferAttribute(VertexBufferPtr vertexBuffer, ComponentDatatype componentDatatype, int numberOfComponents, bool normalize = false, int offsetInBytes = 0, int strideInBytes = 0)
			: m_vertexBuffer(vertexBuffer), m_componentDatatype(componentDatatype), m_numberOfComponents(numberOfComponents), m_normalize(normalize), m_offsetInBytes(offsetInBytes)
		{
			LOG_ASSERT(!((numberOfComponents <= 0)), "numberOfComponents must be greater than zero.");
			LOG_ASSERT(!(offsetInBytes < 0), "offsetInBytes must be greater than or equal to zero.");
			LOG_ASSERT(!(strideInBytes < 0), "stride must be greater than or equal to zero.");

			if (strideInBytes == 0)
			{
				//
				// Tightly packed
				//
				m_strideInBytes = numberOfComponents * VertexArraySizes::SizeOf(componentDatatype);
			}
			else
			{
				m_strideInBytes = strideInBytes;
			}
		}

		VertexBufferPtr GetVertexBuffer(void)
		{
			return m_vertexBuffer;
		}

		ComponentDatatype GetComponentDatatype(void)
		{
			return m_componentDatatype;
		}

		int GetNumberOfComponents(void)
		{
			return m_numberOfComponents;
		}

		bool Normalize(void)
		{
			return m_normalize;
		}

		int GetOffsetInBytes(void)
		{
			return m_offsetInBytes;
		}

		int GetStrideInBytes(void)
		{
			return m_strideInBytes;
		}

	private:
		const VertexBufferPtr	m_vertexBuffer;
		const ComponentDatatype	m_componentDatatype;
		const int				m_numberOfComponents;
		const bool				m_normalize;
		const int				m_offsetInBytes;
		int						m_strideInBytes;
	};

}
