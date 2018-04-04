#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowRenderDevice.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <queue>    // For std::queue

namespace bow {

	class D3DRenderDevice : public IRenderDevice
	{
		friend class D3DRenderContext;
		friend class D3DVertexBuffer;
	public:
		D3DRenderDevice(void);
		~D3DRenderDevice(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================

		bool Initialize(void);
		void VRelease(void);

		GraphicsWindowPtr		VCreateWindow(int width, int height, const std::string& title, WindowType type);

		// =========================================================================
		// SHADER STUFF:
		// =========================================================================

		ShaderProgramPtr		VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename);
		ShaderProgramPtr		VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename);

		ShaderProgramPtr		VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource);
		ShaderProgramPtr		VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource);

		MeshBufferPtr			VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint);
		VertexBufferPtr			VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes);
		IndexBufferPtr			VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes);

		WritePixelBufferPtr		VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

		Texture2DPtr			VCreateTexture2D(Texture2DDescription description);
		Texture2DPtr			VCreateTexture2D(ImagePtr image);

		TextureSamplerPtr		VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy);

		// =========================================================================
		// Command Management:
		// =========================================================================

		// Get an available command list from the command queue.
		ComPtr<ID3D12GraphicsCommandList2> GetCopyCommandList();
		// Returns the fence value to wait for for this command list.
		uint64_t ExecuteCopyCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList);

		void WaitForCopyFenceValue(uint64_t fanceValue);

		// ==========================
		// Static Helper Functions

		static bool CheckTearingSupport();
		static ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
		static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
		
		static ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		static ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);

		// synchronization methods
		static ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device, uint64_t fenceValue = 0);
		static HANDLE CreateEventHandle();
		static uint64_t Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue);
		static void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
		static bool IsFenceComplete(ComPtr<ID3D12Fence> fence, uint64_t fenceValue);

		// ==========================

	private:
		//you shall not copy
		D3DRenderDevice(D3DRenderDevice&) {}
		D3DRenderDevice& operator=(const D3DRenderDevice&) { return *this; }

		bool PrepareCommandQueue();

		ComPtr<ID3D12Device2>		m_d3d12device;

		ComPtr<ID3D12CommandQueue>	m_copyCommandQueue;
		ComPtr<ID3D12Fence>			m_copyFence;
		UINT64						m_copyFenceValue;
		HANDLE						m_copyFenceEvent;

		// FanceValue-AllocatorIndex Pair
		std::queue<std::pair<uint64_t, ID3D12CommandAllocator*>>	m_availableCopyCommandAllocatorsQueue;
		std::queue<ComPtr<ID3D12GraphicsCommandList2>>				m_availableCopyCommandListsQueue;

		// Please do not access this list. This is just to keep the reference counter alive
		std::vector<ComPtr<ID3D12CommandAllocator>> m_allAllocators;

		HINSTANCE m_hInstance;
		bool m_useWarpDevice;
		bool m_initialized;
	};


}
