#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	class D3DVertexBufferAttributes
	{
	public:
		D3DVertexBufferAttributes() {}
		VertexBufferAttributeMap GetAttributes();

		VertexBufferAttributePtr GetAttribute(int Location);
		void SetAttribute(int Location, VertexBufferAttributePtr attribute);

		int MaximumArrayIndex();

	private:
		static inline int NumberOfVertices(VertexBufferAttributePtr attribute);

		D3DVertexBufferAttributeMap m_Attributes;
		int							m_count;
	};

}
