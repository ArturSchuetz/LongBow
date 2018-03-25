#include "BowD3D12RenderContext.h"
#include "BowLogger.h"

#include "BowMeshAttribute.h"
#include "BowClearState.h"

#include "BowD3D12RenderDevice.h"


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

	D3DRenderContext::D3DRenderContext(HWND hWnd, D3DRenderDevice* parent) :
		m_swapChain(nullptr),
		m_commandQueue(nullptr),
		m_RTVDescriptorHeap(nullptr),
		m_rtvDescriptorSize(0),
		m_pipelineState(nullptr),
		m_commandList(nullptr),
		m_frameIndex(0),
		m_fence(nullptr),
		m_FenceValue(0),
		m_fenceEvent(NULL),
		m_hWnd(hWnd),
		m_parentDevice(parent),
		m_initialized(false)
	{

	}

	bool D3DRenderContext::Initialize(unsigned int width, unsigned int height)
	{
		HRESULT hresult;

		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hresult = m_parentDevice->m_D3D12device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateCommandQueue: ") + errorMsg).c_str());
			return false;
		}

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_frameCount;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain = nullptr;
		hresult = m_parentDevice->m_factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
			m_hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateSwapChainForHwnd: ") + errorMsg).c_str());
			return false;
		}

		// This sample does not support fullscreen transitions.
		hresult = m_parentDevice->m_factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("MakeWindowAssociation: ") + errorMsg).c_str());
			return false;
		}

		hresult = swapChain.As(&m_swapChain);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(errorMsg.c_str());
			return false;
		}

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		// Create descriptor heaps.
		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			hresult = m_parentDevice->m_D3D12device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateDescriptorHeap: ") + errorMsg).c_str());
				return false;
			}

			m_rtvDescriptorSize = m_parentDevice->m_D3D12device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		// Create frame resources.
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV for each frame.
			for (UINT n = 0; n < m_frameCount; n++)
			{
				ComPtr<ID3D12Resource> backBuffer;
				hresult = m_swapChain->GetBuffer(n, IID_PPV_ARGS(&backBuffer));
				if (FAILED(hresult))
				{
					std::string errorMsg = toErrorString(hresult);
					LOG_ERROR(std::string(std::string("MakeWindowAssociation: ") + errorMsg).c_str());
					return false;
				}

				m_parentDevice->m_D3D12device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

				m_renderTargets[n] = backBuffer;

				rtvHandle.Offset(1, m_rtvDescriptorSize);
			}
		}
		
		for (UINT n = 0; n < m_frameCount; n++)
		{
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			hresult = m_parentDevice->m_D3D12device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateCommandAllocator: ") + errorMsg).c_str());
				return false;
			}
			m_CommandAllocators[n] = commandAllocator;
		}
		
		// Create the command list.
		hresult = m_parentDevice->m_D3D12device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[m_frameIndex].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateCommandAllocator: ") + errorMsg).c_str());
			return false;
		}

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Close: ") + errorMsg).c_str());
			return false;
		}

		// Create synchronization objects.
		{
			hresult = m_parentDevice->m_D3D12device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateFence: ") + errorMsg).c_str());
				return false;
			}

			// Create an event handle to use for frame synchronization.
			m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fenceEvent == nullptr)
			{
				hresult = HRESULT_FROM_WIN32(GetLastError());
				if (FAILED(hresult))
				{
					std::string errorMsg = toErrorString(hresult);
					LOG_ERROR(errorMsg.c_str());
					return false;
				}
			}
		}
		return true;
	}

	D3DRenderContext::~D3DRenderContext(void)
	{
		VRelease();
	}

	void D3DRenderContext::VRelease(void)
	{
		m_FrameFenceValues[m_frameIndex] = ++m_FenceValue;
		HRESULT hresult = m_commandQueue->Signal(m_fence.Get(), m_FrameFenceValues[m_frameIndex]);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Signal: ") + errorMsg).c_str());
		}

		std::chrono::milliseconds duration = std::chrono::milliseconds::max();
		if (m_fence->GetCompletedValue() < m_FenceValue)
		{
			m_fence->SetEventOnCompletion(m_FenceValue, m_fenceEvent);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("SetEventOnCompletion: ") + errorMsg).c_str());
			}
			::WaitForSingleObject(m_fenceEvent, static_cast<DWORD>(duration.count()));
		}
		::CloseHandle(m_fenceEvent);
	}

	VertexArrayPtr D3DRenderContext::VCreateVertexArray(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
	{
		return VertexArrayPtr(nullptr);
	}

	VertexArrayPtr D3DRenderContext::VCreateVertexArray(MeshBufferPtr meshBuffers)
	{
		return VertexArrayPtr(nullptr);
	}

	VertexArrayPtr D3DRenderContext::VCreateVertexArray()
	{
		return VertexArrayPtr(nullptr);
	}

	FramebufferPtr D3DRenderContext::VCreateFramebuffer()
	{
		return FramebufferPtr(nullptr);
	}

	void D3DRenderContext::VClear(ClearState clearState)
	{
		auto commandAllocator = m_CommandAllocators[m_frameIndex];
		auto backBuffer = m_renderTargets[m_frameIndex];

		commandAllocator->Reset();
		m_commandList->Reset(commandAllocator.Get(), nullptr);

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			m_commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);

			m_commandList->ClearRenderTargetView(rtv, clearState.Color.a, 0, nullptr);
		}
	}

	void D3DRenderContext::VDraw(PrimitiveType primitiveType, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
	{

	}

	void D3DRenderContext::VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
	{

	}

	void D3DRenderContext::VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end)
	{

	}

	void D3DRenderContext::Draw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
	{

	}

	void D3DRenderContext::VSetTexture(int location, Texture2DPtr texture)
	{

	}

	void D3DRenderContext::VSetTextureSampler(int location, TextureSamplerPtr sampler)
	{

	}

	void D3DRenderContext::VSetFramebuffer(FramebufferPtr framebufer)
	{

	}

	void D3DRenderContext::VSetViewport(Viewport viewport)
	{

	}

	Viewport D3DRenderContext::VGetViewport(void)
	{
		return Viewport();
	}

	void D3DRenderContext::VSwapBuffers(bool vsync)
	{
		HRESULT hresult;

		auto backBuffer = m_renderTargets[m_frameIndex];

		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &barrier);

		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Close: ") + errorMsg).c_str());
		}

		ID3D12CommandList* const commandLists[] = {
			m_commandList.Get()
		};
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		m_FrameFenceValues[m_frameIndex] = ++m_FenceValue;
		hresult = m_commandQueue->Signal(m_fence.Get(), m_FrameFenceValues[m_frameIndex]);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Signal: ") + errorMsg).c_str());
		}

		hresult = m_swapChain->Present(1, 0);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Present: ") + errorMsg).c_str());
		}

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		std::chrono::milliseconds duration = std::chrono::milliseconds::max();
		if (m_fence->GetCompletedValue() < m_FrameFenceValues[m_frameIndex])
		{
			hresult = m_fence->SetEventOnCompletion(m_FrameFenceValues[m_frameIndex], m_fenceEvent);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("SetEventOnCompletion: ") + errorMsg).c_str());
			}

			::WaitForSingleObject(m_fenceEvent, static_cast<DWORD>(duration.count()));
		}
	}

}
