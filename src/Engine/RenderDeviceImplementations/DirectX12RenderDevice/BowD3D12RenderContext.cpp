#include "BowD3D12RenderContext.h"
#include "BowLogger.h"

#include "BowVertexBufferAttribute.h"
#include "BowMeshAttribute.h"
#include "BowClearState.h"

#include "BowD3D12VertexBufferAttributes.h"
#include "BowD3D12ShaderProgram.h"
#include "BowD3D12RenderDevice.h"
#include "BowD3D12VertexArray.h"
#include "BowD3D12VertexBuffer.h"
#include "BowD3D12TypeConverter.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"
#include "DirectXMath.h"

#include <algorithm> // For std::min and std::max.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

struct Vertex
{
	float position[3];
	float color[4];
};

namespace bow {

	extern std::string widestring2string(const std::wstring& wstr);
	extern std::wstring string2widestring(const std::string& s);
	extern std::string toErrorString(HRESULT hresult);

	D3DRenderContext::D3DRenderContext(HWND hWnd, D3DRenderDevice* parent) :
		m_swapChain(nullptr),
		m_backBuffers(),
		m_renderTargetViewDescriptorHeap(nullptr),
		m_renderTargetViewDescriptorSize(0),
		m_depthStencilViewDescriptorHeap(nullptr),
		m_depthStencilViewDescriptorSize(0),
		m_currentBackBufferIndex(0),
		m_directCommandQueue(nullptr),
		m_commandAllocators(),
		m_commandList(nullptr),
		m_fence(nullptr),
		m_fenceValue(0),
		m_fenceEvent(nullptr),
		m_hWnd(hWnd),
		m_parentDevice(parent),
		m_initialized(false),
		m_VSync(true),
		m_TearingSupported(false)
	{

	}

	bool D3DRenderContext::Initialize(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;

		HRESULT hresult;

		m_TearingSupported = D3DRenderDevice::CheckTearingSupport();

		m_directCommandQueue = D3DRenderDevice::CreateCommandQueue(m_parentDevice->m_d3d12device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (m_directCommandQueue == nullptr)
			return false;

		m_swapChain = D3DRenderDevice::CreateSwapChain(m_hWnd, m_directCommandQueue, width, height, m_numBuffers);
		if (m_swapChain == nullptr)
			return false;

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_renderTargetViewDescriptorHeap = D3DRenderDevice::CreateDescriptorHeap(m_parentDevice->m_d3d12device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_numBuffers);
		if (m_renderTargetViewDescriptorHeap == nullptr)
			return false;

		m_depthStencilViewDescriptorHeap = D3DRenderDevice::CreateDescriptorHeap(m_parentDevice->m_d3d12device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_numBuffers);
		if (m_depthStencilViewDescriptorHeap == nullptr)
			return false;

		m_renderTargetViewDescriptorSize = m_parentDevice->m_d3d12device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_depthStencilViewDescriptorSize = m_parentDevice->m_d3d12device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		UpdateBackBuffers();
		
		for (UINT n = 0; n < m_numBuffers; n++)
		{
			m_commandAllocators[n] = D3DRenderDevice::CreateCommandAllocator(m_parentDevice->m_d3d12device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		}
		
		m_commandList = D3DRenderDevice::CreateCommandList(m_parentDevice->m_d3d12device, m_commandAllocators[m_currentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (m_commandList == nullptr)
			return false;

		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Close: ") + errorMsg).c_str());
		}

		// Create synchronization objects.
		m_fence = D3DRenderDevice::CreateFence(m_parentDevice->m_d3d12device);
		if (m_fence == nullptr)
			return false;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = D3DRenderDevice::CreateEventHandle();
		if (m_fenceEvent == nullptr)
			return false;

		// TODO: Find an new method for this guys
		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
		m_scissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

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
		return VertexArrayPtr(new D3DVertexArray());
	}

	FramebufferPtr D3DRenderContext::VCreateFramebuffer()
	{
		return FramebufferPtr(nullptr);
	}

	void D3DRenderContext::VClear(ClearState clearState)
	{
		HRESULT hresult;

		// Before any commands can be recorded into the command list, the command allocatorand command list needs to be reset to its inital state
		hresult = m_commandAllocators[m_currentBackBufferIndex]->Reset();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("commandAllocator->Reset: ") + errorMsg).c_str());
		}

		hresult = m_commandList->Reset(m_commandAllocators[m_currentBackBufferIndex].Get(), nullptr);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Reset: ") + errorMsg).c_str());
		}

		// ensure that resource is in correct state before using them (Transition from Present to RenderTarget has to be completed)
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[m_currentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &barrier);

		// Fetch renderTargetView from renderTargetViewDescriptorHeap by offset (m_currentBackBufferIndex) and stride size (m_rtvDescriptorSize)
		CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetView(m_renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_renderTargetViewDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilView(m_depthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_depthStencilViewDescriptorSize);

		m_commandList->ClearRenderTargetView(renderTargetView, clearState.Color.a, 0, nullptr);
		m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Close: ") + errorMsg).c_str());
		}

		// The command list is executed. ExecuteCommandLists only takes a list of command lists to be executed
		ID3D12CommandList* const commandLists[] = {
			m_commandList.Get()
		};
		m_directCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		Flush();
	}

	void D3DRenderContext::VDraw(PrimitiveType primitiveType, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
	{
		VDraw(primitiveType, 0, std::dynamic_pointer_cast<D3DVertexArray>(vertexArray)->GetMaximumArrayIndex() + 1, vertexArray, shaderProgram, renderState);
	}

	void D3DRenderContext::VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
	{
		HRESULT hresult;

		CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetView(m_renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_renderTargetViewDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilView(m_depthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_depthStencilViewDescriptorSize);

		D3DShaderProgramPtr d3d12shaderProgram = std::dynamic_pointer_cast<D3DShaderProgram>(shaderProgram);
		ComPtr<ID3D12PipelineState> pipelineState = d3d12shaderProgram->GetPipelineState(vertexArray);
		ComPtr<ID3D12RootSignature> rootsignature = d3d12shaderProgram->GetRootSignature();

		auto attributes = vertexArray->VGetAttributes();
		std::vector<D3D12_VERTEX_BUFFER_VIEW> bufferViews(attributes.size());
		for (unsigned int i = 0; i < attributes.size(); i++)
		{
			D3DVertexBufferPtr vertexBuffer = std::dynamic_pointer_cast<D3DVertexBuffer>(attributes[i]->GetVertexBuffer());
			bufferViews[i].BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			bufferViews[i].SizeInBytes = vertexBuffer->VGetSizeInBytes();
			bufferViews[i].StrideInBytes = attributes[i]->GetStrideInBytes();
		};

		hresult = m_commandAllocators[m_currentBackBufferIndex]->Reset();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("commandAllocator->Reset: ") + errorMsg).c_str());
		}

		hresult = m_commandList->Reset(m_commandAllocators[m_currentBackBufferIndex].Get(), pipelineState.Get());
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Reset: ") + errorMsg).c_str());
		}

		m_commandList->SetGraphicsRootSignature(rootsignature.Get());


		m_commandList->IASetPrimitiveTopology(D3DTypeConverter::To(primitiveType));
		m_commandList->IASetVertexBuffers(0, bufferViews.size(), &bufferViews[0]);

		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);

		m_commandList->OMSetRenderTargets(1, &renderTargetView, FALSE, &depthStencilView);

		m_commandList->DrawInstanced(count, 1, offset, 0);

		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Close: ") + errorMsg).c_str());
		}

		// The command list is executed. ExecuteCommandLists only takes a list of command lists to be executed
		ID3D12CommandList* const commandLists[] = {
			m_commandList.Get()
		};
		m_directCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		Flush();
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
		m_viewport = CD3DX12_VIEWPORT(static_cast<float>(viewport.x), static_cast<float>(viewport.y), static_cast<float>(viewport.width), static_cast<float>(viewport.height));
		m_scissorRect = CD3DX12_RECT(static_cast<LONG>(viewport.x), static_cast<LONG>(viewport.y), static_cast<LONG>(viewport.width), static_cast<LONG>(viewport.height));
	}

	Viewport D3DRenderContext::VGetViewport(void)
	{
		return Viewport(static_cast<int>(m_viewport.TopLeftX), static_cast<int>(m_viewport.TopLeftY), static_cast<int>(m_viewport.Width), static_cast<int>(m_viewport.Height));
	}

	void D3DRenderContext::VSwapBuffers(bool vsync)
	{
		HRESULT hresult;

		// Create the command list.
		hresult = m_commandAllocators[m_currentBackBufferIndex]->Reset();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("commandAllocator->Reset: ") + errorMsg).c_str());
		}

		hresult = m_commandList->Reset(m_commandAllocators[m_currentBackBufferIndex].Get(), nullptr);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Reset: ") + errorMsg).c_str());
		}

		// ensure that resource is in correct state before using them (Transition from RenderTarget to Present has to be completed)
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[m_currentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &barrier);

		// This method must be called on the command list before executed on the command queue
		hresult = m_commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_commandList->Close: ") + errorMsg).c_str());
		}

		// The command list is executed. ExecuteCommandLists only takes a list of command lists to be executed
		ID3D12CommandList* const commandLists[] = {
			m_commandList.Get()
		};
		m_directCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		// Signal is inserted to stall the CPU thread until any resources are finished being used
		m_frameFenceValues[m_currentBackBufferIndex] = D3DRenderDevice::Signal(m_directCommandQueue, m_fence, m_fenceValue);

		UINT syncInterval = m_VSync ? 1 : 0;
		UINT presentFlags = m_TearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

		// Present the back buffer to the screen
		hresult = m_swapChain->Present(syncInterval, presentFlags);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_swapChain->Present: ") + errorMsg).c_str());
		}

		// after presenting, the index of the current back buffer is updated
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		// Before overwriting the contents of the current back buffer with the content of the next frame, the CPU thread is stalled
		D3DRenderDevice::WaitForFenceValue(m_fence, m_frameFenceValues[m_currentBackBufferIndex], m_fenceEvent);
	}

	// -------------------------------------------------------------------------------------- //
	// PRIVATE METHODS
	// -------------------------------------------------------------------------------------- //

	void D3DRenderContext::Resize(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;

		HRESULT hresult;

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		Flush();

		for (int i = 0; i < m_numBuffers; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			m_backBuffers[i].Reset();
			m_depthBuffers[i].Reset();
			m_frameFenceValues[i] = m_frameFenceValues[m_currentBackBufferIndex];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		hresult = m_swapChain->GetDesc(&swapChainDesc);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_swapChain->GetDesc: ") + errorMsg).c_str());
		}

		hresult = m_swapChain->ResizeBuffers(m_numBuffers, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("m_swapChain->ResizeBuffers: ") + errorMsg).c_str());
		}

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		UpdateBackBuffers();
	}

	void D3DRenderContext::UpdateBackBuffers()
	{
		// Create frame resources.
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < m_numBuffers; n++)
		{
			ComPtr<ID3D12Resource> backBuffer;
			HRESULT hresult = m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_backBuffers[n]));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("m_swapChain->GetBuffer: ") + errorMsg).c_str());
				return;
			}

			m_parentDevice->m_d3d12device->CreateRenderTargetView(m_backBuffers[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_renderTargetViewDescriptorSize);
		}

		int width = std::max((unsigned int)1, m_width);
		int height = std::max((unsigned int)1, m_height);

		// Resize screen dependent resources.
		// Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_depthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < m_numBuffers; n++)
		{
			HRESULT hresult = m_parentDevice->m_d3d12device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&optimizedClearValue,
				IID_PPV_ARGS(&m_depthBuffers[n])
			);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("m_d3d12device->CreateCommittedResource: ") + errorMsg).c_str());
				return;
			}

			// Update the depth-stencil view.
			m_parentDevice->m_d3d12device->CreateDepthStencilView(m_depthBuffers[n].Get(), &dsv, dsvHandle);
			dsvHandle.Offset(1, m_depthStencilViewDescriptorSize);
		}
	}

	void D3DRenderContext::Flush()
	{
		uint64_t fenceValueForSignal = D3DRenderDevice::Signal(m_directCommandQueue, m_fence, m_fenceValue);
		D3DRenderDevice::WaitForFenceValue(m_fence, fenceValueForSignal, m_fenceEvent);
	}

}
