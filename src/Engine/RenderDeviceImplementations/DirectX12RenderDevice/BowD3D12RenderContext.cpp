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
		m_backBuffers(),
		m_commandQueue(nullptr),
		m_rtvDescriptorHeap(nullptr),
		m_pipelineState(nullptr),
		m_commandAllocators(),
		m_commandList(nullptr),
		m_fence(nullptr),
		m_fenceValue(0),
		m_fenceEvent(nullptr),
		m_rtvDescriptorSize(0),
		m_currentBackBufferIndex(0),
		m_hWnd(hWnd),
		m_parentDevice(parent),
		m_initialized(false),
		m_VSync(true),
		m_TearingSupported(false)
	{

	}

	bool D3DRenderContext::Initialize(unsigned int width, unsigned int height)
	{
		m_TearingSupported = CheckTearingSupport();

		m_commandQueue = CreateCommandQueue(m_parentDevice->m_D3D12device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (m_commandQueue == nullptr)
			return false;

		m_swapChain = CreateSwapChain(m_hWnd, m_commandQueue, width, height, m_bufferCount);
		if (m_swapChain == nullptr)
			return false;

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_rtvDescriptorHeap = CreateDescriptorHeap(m_parentDevice->m_D3D12device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_bufferCount);
		if (m_rtvDescriptorHeap == nullptr)
			return false;

		m_rtvDescriptorSize = m_parentDevice->m_D3D12device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		UpdateBackBuffers();
		
		for (UINT n = 0; n < m_bufferCount; n++)
		{
			m_commandAllocators[n] = CreateCommandAllocator(m_parentDevice->m_D3D12device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		}
		
		m_commandList = CreateCommandList(m_parentDevice->m_D3D12device, m_commandAllocators[m_currentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (m_commandList == nullptr)
			return false;

		// Create synchronization objects.
		m_fence = CreateFence(m_parentDevice->m_D3D12device);
		if (m_fence == nullptr)
			return false;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEventHandle();
		if (m_fenceEvent == nullptr)
			return false;

		m_initialized = true;
		return true;
	}

	D3DRenderContext::~D3DRenderContext(void)
	{
		VRelease();
	}

	void D3DRenderContext::VRelease(void)
	{
		Flush();
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
		auto commandAllocator = m_commandAllocators[m_currentBackBufferIndex];
		auto backBuffer = m_backBuffers[m_currentBackBufferIndex];

		// Before any commands can be recorded into the command list, the command allocatorand command list needs to be reset to its inital state
		commandAllocator->Reset();
		m_commandList->Reset(commandAllocator.Get(), nullptr);

		// ensure that resource is in correct state before using them (Transition from Present to RenderTarget has to be completed)
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &barrier);

		// Fetch renderTargetView from renderTargetViewDescriptorHeap by offset (m_currentBackBufferIndex) and stride size (m_rtvDescriptorSize)
		CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetView(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_rtvDescriptorSize);
		m_commandList->ClearRenderTargetView(renderTargetView, clearState.Color.a, 0, nullptr);
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

		auto backBuffer = m_backBuffers[m_currentBackBufferIndex];

		// ensure that resource is in correct state before using them (Transition from RenderTarget to Present has to be completed)
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &barrier);

		// This method must be called on the command list before executed on the command queue
		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Close: ") + errorMsg).c_str());
		}

		// The command list is executed. ExecuteCommandLists only takes a list of command lists to be executed
		ID3D12CommandList* const commandLists[] = {
			m_commandList.Get()
		};
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		// Signal is inserted to stall the CPU thread until any resources are finished being used
		m_frameFenceValues[m_currentBackBufferIndex] = Signal(m_commandQueue, m_fence, m_fenceValue);

		UINT syncInterval = m_VSync ? 1 : 0;
		UINT presentFlags = m_TearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

		// Present the back buffer to the screen
		hresult = m_swapChain->Present(syncInterval, presentFlags);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Present: ") + errorMsg).c_str());
		}

		// after presenting, the index of the current back buffer is updated
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		// Before overwriting the contents of the current back buffer with the content of the next frame, the CPU thread is stalled
		WaitForFenceValue(m_fence, m_frameFenceValues[m_currentBackBufferIndex], m_fenceEvent);
	}

	// -------------------------------------------------------------------------------------- //
	// PRIVATE METHODS
	// -------------------------------------------------------------------------------------- //

	void D3DRenderContext::Resize(unsigned int width, unsigned int height)
	{
		HRESULT hresult;

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		Flush();

		for (int i = 0; i < m_bufferCount; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			m_backBuffers[i].Reset();
			m_frameFenceValues[i] = m_frameFenceValues[m_currentBackBufferIndex];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		hresult = m_swapChain->GetDesc(&swapChainDesc);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("GetDesc: ") + errorMsg).c_str());
		}

		hresult = m_swapChain->ResizeBuffers(m_bufferCount, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("ResizeBuffers: ") + errorMsg).c_str());
		}

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		UpdateBackBuffers();
	}

	void D3DRenderContext::UpdateBackBuffers()
	{
		// Create frame resources.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT nBuffer = 0; nBuffer < m_bufferCount; nBuffer++)
		{
			ComPtr<ID3D12Resource> backBuffer;
			HRESULT hresult = m_swapChain->GetBuffer(nBuffer, IID_PPV_ARGS(&backBuffer));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("GetBuffer: ") + errorMsg).c_str());
				return;
			}

			m_parentDevice->m_D3D12device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

			m_backBuffers[nBuffer] = backBuffer;

			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	void D3DRenderContext::Flush()
	{
		uint64_t fenceValueForSignal = Signal(m_commandQueue, m_fence, m_fenceValue);
		WaitForFenceValue(m_fence, fenceValueForSignal, m_fenceEvent);
	}

}
