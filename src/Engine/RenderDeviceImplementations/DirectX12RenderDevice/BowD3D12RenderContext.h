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

		static const UINT			m_numBuffers = 2;

		// Buffer and RenderTargets
		ComPtr<IDXGISwapChain4>				m_swapChain;
		ComPtr<ID3D12Resource>				m_backBuffers[m_numBuffers];
		ComPtr<ID3D12Resource>				m_depthBuffers[m_numBuffers];

		ComPtr<ID3D12DescriptorHeap>		m_renderTargetViewDescriptorHeap;
		UINT								m_renderTargetViewDescriptorSize;

		ComPtr<ID3D12DescriptorHeap>		m_depthStencilViewDescriptorHeap;
		UINT								m_depthStencilViewDescriptorSize;

		UINT								m_currentBackBufferIndex;

		// Render Command Management
		ComPtr<ID3D12CommandQueue>			m_directCommandQueue;
		ComPtr<ID3D12CommandAllocator>		m_commandAllocators[m_numBuffers];
		ComPtr<ID3D12GraphicsCommandList2>	m_commandList;

		// synchronization
		ComPtr<ID3D12Fence>		m_fence;
		UINT64					m_fenceValue;
		UINT64					m_frameFenceValues[m_numBuffers] = {};
		HANDLE					m_fenceEvent;

		D3D12_VIEWPORT	m_viewport;
		D3D12_RECT		m_scissorRect;

		unsigned int m_width;
		unsigned int m_height;

		HWND						m_hWnd;
		D3DRenderDevice*			m_parentDevice;
		bool						m_initialized;
		bool						m_VSync;
		bool						m_TearingSupported;

		D3DTextureUnitsPtr			m_textureUnits;
	};

}
