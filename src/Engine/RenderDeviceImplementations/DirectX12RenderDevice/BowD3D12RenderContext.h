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

		static D3DRenderContext*	m_currentContext;
		static const UINT			m_frameCount = 2;

		ComPtr<IDXGISwapChain4>				m_swapChain;
		ComPtr<ID3D12Resource>				m_renderTargets[m_frameCount];
		ComPtr<ID3D12CommandQueue>			m_commandQueue;
		ComPtr<ID3D12DescriptorHeap>		m_RTVDescriptorHeap;
		UINT								m_rtvDescriptorSize;
		ComPtr<ID3D12PipelineState>			m_pipelineState;
		ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		ComPtr<ID3D12CommandAllocator>		m_CommandAllocators[m_frameCount];

		UINT m_frameIndex;

		ComPtr<ID3D12Fence> m_fence;
		UINT64 m_FenceValue;
		UINT64 m_FrameFenceValues[m_frameCount] = {};
		HANDLE m_fenceEvent;

		HWND m_hWnd;
		D3DRenderDevice*			m_parentDevice;
		bool						m_initialized;
	};

}
