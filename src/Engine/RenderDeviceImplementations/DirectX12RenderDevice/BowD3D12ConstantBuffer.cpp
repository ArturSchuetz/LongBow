#include "BowD3D12ConstantBuffer.h"
#include "BowLogger.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	extern std::string widestring2string(const std::wstring& wstr);
	extern std::wstring string2widestring(const std::string& s);
	extern std::string toErrorString(HRESULT hresult);

	D3DConstantBuffer::D3DConstantBuffer(ID3D12Device2* device, UINT sizeInBytes, UINT bindingPoint) : 
		m_device(device),
		m_sizeInBytes(sizeInBytes),
		m_bindingPoint(bindingPoint)
	{
		m_pBufferData = new char[sizeInBytes];
		memset(m_pBufferData, 0, sizeInBytes);
		m_dirty = false;
	}

	D3DConstantBuffer::~D3DConstantBuffer()
	{
		delete m_pBufferData;
	}

	void D3DConstantBuffer::Initialize()
	{
		// Describe and create a constant buffer view (CBV) descriptor heap.
		// Flags indicate that this descriptor heap can be bound to the pipeline 
		// and that descriptors contained in it can be referenced by a root table.
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		HRESULT hresult = m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateDescriptorHeap: ") + errorMsg).c_str());
		}

		// Create the constant buffer.
		{
			hresult = m_device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_constantBuffer));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateCommittedResource: ") + errorMsg).c_str());
			}

			// Describe and create a constant buffer view.
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = (m_sizeInBytes + 255) & ~255;	// CB size is required to be 256-byte aligned.
			m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

			// Map and initialize the constant buffer. We don't unmap this until the
			// app closes. Keeping things mapped for the lifetime of the resource is okay.
			CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
			hresult = m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("Map: ") + errorMsg).c_str());
			}

			memcpy(m_pCbvDataBegin, m_pBufferData, m_sizeInBytes);
		}
	}

	char* D3DConstantBuffer::SetVariableDeclaration(std::string name, UINT offset, UINT sizeInBytes)
	{
		if(m_variables.find(name) != m_variables.end())
			m_variables[name] = std::pair<UINT, UINT>(offset, sizeInBytes);
		else
			m_variables.insert(std::pair<std::string, std::pair<UINT, UINT>>(name, std::pair<UINT, UINT>(offset, sizeInBytes)));

		return m_pBufferData + offset;
	}

	void D3DConstantBuffer::BindBuffers(ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		if (m_dirty)
		{
			m_dirty = false;
			memcpy(m_pCbvDataBegin, m_pBufferData, m_sizeInBytes);
		}

		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void D3DConstantBuffer::SetDirty()
	{
		m_dirty = true;
	}
}
