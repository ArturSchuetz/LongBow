#include "BowD3D12RenderDevice.h"
#include "BowLogger.h"

#include "BowMeshAttribute.h"
#include "BowClearState.h"

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

	bool D3DRenderDevice::CheckTearingSupport()
	{
		BOOL allowTearing = FALSE;

		// Rather than create the DXGI 1.5 factory interface directly, we create the
		// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
		// graphics debugging tools which will not support the 1.5 factory interface 
		// until a future update.
		ComPtr<IDXGIFactory4> factory4;
		if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
		{
			ComPtr<IDXGIFactory5> factory5;
			if (SUCCEEDED(factory4.As(&factory5)))
			{
				if (FAILED(factory5->CheckFeatureSupport(
					DXGI_FEATURE_PRESENT_ALLOW_TEARING,
					&allowTearing, sizeof(allowTearing))))
				{
					allowTearing = FALSE;
				}
			}
		}

		return allowTearing == TRUE;
	}

	ComPtr<IDXGISwapChain4> D3DRenderDevice::CreateSwapChain(HWND hWnd, ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
	{
		UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		ComPtr<IDXGIFactory4> dxgiFactory4;
		HRESULT hresult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory4));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateDXGIFactory2: ") + errorMsg).c_str());
			return ComPtr<IDXGISwapChain4>(nullptr);
		}

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		// It is recommended to always allow tearing if tearing support is available.
		swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		hresult = dxgiFactory4->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("dxgiFactory4->CreateSwapChainForHwnd: ") + errorMsg).c_str());
			return ComPtr<IDXGISwapChain4>(nullptr);
		}

		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
		// will be handled manually.
		hresult = dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("dxgiFactory4->MakeWindowAssociation: ") + errorMsg).c_str());
			return ComPtr<IDXGISwapChain4>(nullptr);
		}

		ComPtr<IDXGISwapChain4> dxgiSwapChain4;
		hresult = swapChain1.As(&dxgiSwapChain4);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("swapChain1.As: ") + errorMsg).c_str());
			return ComPtr<IDXGISwapChain4>(nullptr);
		}

		return dxgiSwapChain4;
	}

	ComPtr<ID3D12DescriptorHeap> D3DRenderDevice::CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t bufferCount)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = bufferCount;
		desc.Type = type;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		HRESULT hresult = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("device->CreateDescriptorHeap: ") + errorMsg).c_str());
			return ComPtr<ID3D12DescriptorHeap>(nullptr);
		}

		return descriptorHeap;
	}

	ComPtr<ID3D12CommandQueue> D3DRenderDevice::CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	{
		ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.NodeMask = 0;

		HRESULT hresult = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("device->CreateCommandQueue: ") + errorMsg).c_str());
			return ComPtr<ID3D12CommandQueue>(nullptr);
		}

		return d3d12CommandQueue;
	}

	ComPtr<ID3D12CommandAllocator> D3DRenderDevice::CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		HRESULT hresult = device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("device->CreateCommandAllocator: ") + errorMsg).c_str());
			return ComPtr<ID3D12CommandAllocator>(nullptr);
		}

		return commandAllocator;
	}

	ComPtr<ID3D12GraphicsCommandList2> D3DRenderDevice::CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
	{
		ComPtr<ID3D12GraphicsCommandList2> commandList;
		HRESULT hresult = device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("device->CreateCommandList: ") + errorMsg).c_str());
			return ComPtr<ID3D12GraphicsCommandList2>(nullptr);
		}

		return commandList;
	}

	ComPtr<ID3D12Fence> D3DRenderDevice::CreateFence(ComPtr<ID3D12Device2> device, uint64_t fenceValue)
	{
		ComPtr<ID3D12Fence> fence;

		HRESULT hresult = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("device->CreateFence: ") + errorMsg).c_str());
			return ComPtr<ID3D12Fence>(nullptr);
		}

		return fence;
	}

	HANDLE D3DRenderDevice::CreateEventHandle()
	{
		HANDLE fenceEvent = nullptr;

		fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr)
		{
			HRESULT hresult = HRESULT_FROM_WIN32(GetLastError());
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("::CreateEvent: ") + errorMsg).c_str());
				return nullptr;
			}
		}

		return fenceEvent;
	}

	uint64_t D3DRenderDevice::Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue)
	{
		uint64_t fenceValueForSignal = ++fenceValue;
		HRESULT hresult = commandQueue->Signal(fence.Get(), fenceValueForSignal);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("commandQueue->Signal: ") + errorMsg).c_str());
			return -1;
		}

		return fenceValueForSignal;
	}

	void D3DRenderDevice::WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration)
	{
		if (fence->GetCompletedValue() < fenceValue)
		{
			HRESULT hresult = fence->SetEventOnCompletion(fenceValue, fenceEvent);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("fence->SetEventOnCompletion: ") + errorMsg).c_str());
				return;
			}

			::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
		}
	}

	bool D3DRenderDevice::IsFenceComplete(ComPtr<ID3D12Fence> fence, uint64_t fenceValue)
	{
		return fence->GetCompletedValue() >= fenceValue;
	}

}
