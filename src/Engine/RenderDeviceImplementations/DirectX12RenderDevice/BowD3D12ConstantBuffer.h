#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowCleanableObserver.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <map>

namespace bow {

	class D3DConstantBuffer
	{
	public:
		D3DConstantBuffer(ID3D12Device2* device, UINT sizeInBytes, UINT bindingPoint);
		~D3DConstantBuffer();

		void Initialize();

		char* SetVariableDeclaration(std::string name, UINT offset, UINT size);

		void BindBuffers(ComPtr<ID3D12GraphicsCommandList> commandList);
		void SetDirty();

	private:
		//you shall not copy
		D3DConstantBuffer(D3DConstantBuffer&) {}
		D3DConstantBuffer& operator=(const D3DConstantBuffer&) { return *this; }

		ID3D12Device2*					m_device;

		UINT							m_sizeInBytes;
		UINT							m_bindingPoint;

		std::map<std::string, std::pair<UINT, UINT>> m_variables;

		ComPtr<ID3D12DescriptorHeap>	m_cbvHeap;
		ComPtr<ID3D12Resource>			m_constantBuffer;
		UINT8*							m_pCbvDataBegin;
		char*							m_pBufferData;
		bool							m_dirty;
	};

}
