#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowRenderContext.h"
#include "BowRenderState.h"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace bow {

	class D3DRenderContext : public IRenderContext
	{
		friend class D3DRenderDevice;
		friend class D3DGraphicsWindow;
	public:
		D3DRenderContext(HWND hWnd, D3DRenderDevice* parent);
		~D3DRenderContext(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================

		bool	Initialize(unsigned int width, unsigned int height);
		void	VRelease(void);

		// =========================================================================
		// RENDERING STUFF:
		// =========================================================================

		VertexArrayPtr	VCreateVertexArray(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint);
		VertexArrayPtr	VCreateVertexArray(MeshBufferPtr meshBuffers);
		VertexArrayPtr	VCreateVertexArray();
		FramebufferPtr	VCreateFramebuffer();

		void	VClear(struct ClearState clearState);
		void	VDraw(PrimitiveType primitiveType, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState);
		void	VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState);
		void	Draw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState);
		void	VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end);

		void	VSetTexture(int location, Texture2DPtr texture);
		void	VSetTextureSampler(int location, TextureSamplerPtr sampler);
		void	VSetFramebuffer(FramebufferPtr framebufer);
		void	VSetViewport(Viewport viewport);
		Viewport VGetViewport(void);

		void	VSwapBuffers(bool vsync);

	private:
		//you shall not copy
		D3DRenderContext(D3DRenderContext&) {}
		D3DRenderContext& operator=(const D3DRenderContext&) { return *this; }
		
		void Resize(unsigned int width, unsigned int height);
		void UpdateBackBuffers();
		void Flush();

		// ==========================
		// DirectX12 Helper Functions

		static bool CheckTearingSupport();
		static ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		static ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
		static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
		static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		static ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);

		// synchronization methods
		static ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device);
		static HANDLE CreateEventHandle();
		static uint64_t Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue);
		static void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());

		// ==========================

		static D3DRenderContext*	m_currentContext;
		static const UINT			m_bufferCount = 2;

		ComPtr<IDXGISwapChain4>				m_swapChain;
		ComPtr<ID3D12Resource>				m_backBuffers[m_bufferCount];
		ComPtr<ID3D12CommandQueue>			m_commandQueue;
		ComPtr<ID3D12DescriptorHeap>		m_rtvDescriptorHeap;
		ComPtr<ID3D12PipelineState>			m_pipelineState;
		ComPtr<ID3D12CommandAllocator>		m_commandAllocators[m_bufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		ComPtr<ID3D12Fence>					m_fence;

		UINT64 m_fenceValue;
		UINT64 m_frameFenceValues[m_bufferCount] = {};
		HANDLE m_fenceEvent;

		UINT m_rtvDescriptorSize;
		UINT m_currentBackBufferIndex;

		HWND						m_hWnd;
		D3DRenderDevice*			m_parentDevice;
		bool						m_initialized;
		bool						m_VSync;
		bool						m_TearingSupported;
	};

}
