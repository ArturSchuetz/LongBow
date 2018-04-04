#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowFragmentOutputs.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	class D3DFragmentOutputs : public IFragmentOutputs
	{
	public:
		D3DFragmentOutputs();
		~D3DFragmentOutputs();

		int operator[](std::string typeName) const;

	private:
	};

}
