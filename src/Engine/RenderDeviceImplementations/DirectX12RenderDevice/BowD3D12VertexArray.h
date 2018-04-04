#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowVertexArray.h"
#include "BowD3D12VertexBufferAttributes.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	class D3DVertexArray : public IVertexArray
	{
	public:
		D3DVertexArray();
		~D3DVertexArray();

		VertexBufferAttributeMap	VGetAttributes();
		void						VSetAttribute(unsigned int location, VertexBufferAttributePtr pointer);
		void						VSetAttribute(ShaderVertexAttributePtr Location, VertexBufferAttributePtr pointer);

		IndexBufferPtr				VGetIndexBuffer();
		void						VSetIndexBuffer(IndexBufferPtr pointer);

		int							GetMaximumArrayIndex();

	private:
		D3DVertexBufferAttributes	m_Attributes;
		D3DIndexBufferPtr			m_indexBuffer;

		bool						m_dirtyIndexBuffer;
		int							m_maximumArrayIndex;
	};
}
