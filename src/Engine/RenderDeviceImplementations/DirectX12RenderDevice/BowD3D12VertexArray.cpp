#include "BowD3D12VertexArray.h"
#include "BowLogger.h"

#include "BowVertexBufferAttribute.h"
#include "BowShaderVertexAttribute.h"
#include "BowD3D12IndexBuffer.h"

namespace bow {

	D3DVertexArray::D3DVertexArray() : m_dirtyIndexBuffer(false), m_maximumArrayIndex(0)
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
		return std::dynamic_pointer_cast<IIndexBuffer>(m_indexBuffer);
	}

	void D3DVertexArray::VSetIndexBuffer(IndexBufferPtr indexBufferPtr)
	{
		m_indexBuffer = std::dynamic_pointer_cast<D3DIndexBuffer>(indexBufferPtr);
		m_dirtyIndexBuffer = true;
	}

	int D3DVertexArray::GetMaximumArrayIndex()
	{
		return m_maximumArrayIndex;
	}

}
