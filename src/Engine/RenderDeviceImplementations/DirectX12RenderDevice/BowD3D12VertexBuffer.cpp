#include "BowD3D12VertexBuffer.h"
#include "BowLogger.h"

#include "BowD3D12RenderDevice.h"
#include "BowBufferHint.h"

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

	D3DVertexBuffer::D3DVertexBuffer(D3DRenderDevice* device, BufferHint usageHint, int sizeInBytes) :
		m_device(device),
		m_usageHint(usageHint),
		m_sizeInBytes(sizeInBytes)
	{
		// Create a committed resource for the GPU resource in a default heap.
		HRESULT hresult = m_device->m_d3d12device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_buffer.GetAddressOf()));

		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateCommittedResource: ") + errorMsg).c_str());
		}
	}

	D3DVertexBuffer::~D3DVertexBuffer()
	{

	}

	void D3DVertexBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		HRESULT hresult;

		ComPtr<ID3D12Resource> intermediateVertexBuffer;
		// Create an committed resource for the upload.
		if (bufferInSystemMemory)
		{
			hresult = m_device->m_d3d12device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(lengthInBytes),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&intermediateVertexBuffer));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateCommittedResource: ") + errorMsg).c_str());
			}

			ComPtr<ID3D12GraphicsCommandList2> commandList = m_device->GetCopyCommandList();

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferInSystemMemory;
			subresourceData.RowPitch = lengthInBytes;
			subresourceData.SlicePitch = subresourceData.RowPitch;
			UpdateSubresources(commandList.Get(), m_buffer.Get(), intermediateVertexBuffer.Get(), destinationOffsetInBytes, 0, 1, &subresourceData);
			
			// This can be implemented deferred (collect all commands and execute them all together instead of executing them one by one)
			auto fenceValue = m_device->ExecuteCopyCommandList(commandList);
			m_device->WaitForCopyFenceValue(fenceValue);
		}
	}

	std::shared_ptr<void> D3DVertexBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		LOG_FATAL("Not yet implemented!");
		return std::shared_ptr<void>(nullptr);
	}

	int D3DVertexBuffer::VGetSizeInBytes()
	{
		return m_sizeInBytes;
	}

	BufferHint D3DVertexBuffer::VGetUsageHint()
	{
		return m_usageHint;
	}

	D3D12_GPU_VIRTUAL_ADDRESS D3DVertexBuffer::GetGPUVirtualAddress()
	{
		return m_buffer->GetGPUVirtualAddress();
	}

}
