#include "BowD3D12VertexArray.h"
#include "BowLogger.h"

#include "IBowIndexBuffer.h"

#include "BowVertexBufferAttribute.h"
#include "BowShaderVertexAttribute.h"
#include "BowD3D12VertexBufferAttributes.h"


namespace bow {

	D3DVertexArray::D3DVertexArray() : m_maximumArrayIndex(0)
	{

	}

	D3DVertexArray::~D3DVertexArray()
	{

	}

	VertexBufferAttributeMap D3DVertexArray::VGetAttributes()
	{
		return m_Attributes.GetAttributes();
	}

	void D3DVertexArray::VSetAttribute(unsigned int location, VertexBufferAttributePtr pointer)
	{
		m_Attributes.SetAttribute(location, pointer);
		m_maximumArrayIndex = m_Attributes.MaximumArrayIndex();
	}

	void D3DVertexArray::VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer)
	{
		m_Attributes.SetAttribute(vertexAttribute->Location, pointer);
		m_maximumArrayIndex = m_Attributes.MaximumArrayIndex();
	}

	IndexBufferPtr D3DVertexArray::VGetIndexBuffer()
	{
		LOG_FATAL("Not yet implemented!");
		return IndexBufferPtr(nullptr);
	}

	void D3DVertexArray::VSetIndexBuffer(IndexBufferPtr pointer)
	{
		LOG_FATAL("Not yet implemented!");
	}

	int D3DVertexArray::GetMaximumArrayIndex()
	{
		return m_maximumArrayIndex;
	}

}
