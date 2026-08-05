#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <DirectX12ThinDevice/BowD3D12ThinObjects.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{
namespace
{

const uint32_t MaxRenderTargetViews = 64;
const uint32_t MaxDepthStencilViews = 16;

D3D12_COMMAND_LIST_TYPE ToCommandListType(ThinQueueType type)
{
    switch (type)
    {
    case ThinQueueType::Compute:
        return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    case ThinQueueType::Transfer:
        return D3D12_COMMAND_LIST_TYPE_COPY;
    default:
        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

} // namespace

D3D12ThinDevice::D3D12ThinDevice() : m_renderTargetSize(0), m_depthStencilSize(0), m_renderTargetsUsed(0), m_depthStencilsUsed(0), m_initialized(false) { FN("D3D12ThinDevice::D3D12ThinDevice"); }

D3D12ThinDevice::~D3D12ThinDevice()
{
    FN("D3D12ThinDevice::~D3D12ThinDevice");

    VRelease();
}

bool D3D12ThinDevice::Initialize(uint32_t deviceHandle)
{
    FN("D3D12ThinDevice::Initialize");

    UINT factoryFlags = 0;

#ifdef _DEBUG
    // The debug layer has to be enabled before the device is created, and is
    // the DirectX counterpart of the Vulkan validation layer.
    Microsoft::WRL::ComPtr<ID3D12Debug> debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
    {
        debug->EnableDebugLayer();
        factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
    else
    {
        LOG_WARNING("The DirectX debug layer is not installed; running without it.");
    }
#endif

    if (!D3D12Check(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory)), "CreateDXGIFactory2"))
    {
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    if (deviceHandle != 0)
    {
        m_factory->EnumAdapters1(deviceHandle, &adapter);
    }
    if (adapter == nullptr)
    {
        // Ask DXGI for the highest-performance adapter rather than the first
        // one enumerated, which on a laptop is usually the integrated GPU.
        m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
    }

    // Feature level 11.0 is the floor for DirectX 12 itself; 12.0 is required
    // for the resource binding tier the descriptor heaps here assume.
    if (!D3D12Check(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)), "D3D12CreateDevice"))
    {
        return false;
    }

    if (adapter != nullptr)
    {
        DXGI_ADAPTER_DESC1 description = {};
        if (SUCCEEDED(adapter->GetDesc1(&description)))
        {
            char name[128] = {0};
            WideCharToMultiByte(CP_UTF8, 0, description.Description, -1, name, sizeof(name) - 1, nullptr, nullptr);
            m_capabilities.adapterName = name;
            LOG_INFO("DirectX 12 on %s", name);
        }
    }

    // Root constants are limited to 64 DWORDs across the whole signature,
    // which is the same order as Vulkan's push constant limit.
    m_capabilities.maxRootConstantBytes = 64 * 4;

    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
    if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
    {
        m_capabilities.rayTracing = (options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0);
    }

    D3D12_DESCRIPTOR_HEAP_DESC renderTargetHeap = {};
    renderTargetHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    renderTargetHeap.NumDescriptors = MaxRenderTargetViews;
    if (!D3D12Check(m_device->CreateDescriptorHeap(&renderTargetHeap, IID_PPV_ARGS(&m_renderTargetHeap)), "CreateDescriptorHeap (RTV)"))
    {
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC depthStencilHeap = {};
    depthStencilHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    depthStencilHeap.NumDescriptors = MaxDepthStencilViews;
    if (!D3D12Check(m_device->CreateDescriptorHeap(&depthStencilHeap, IID_PPV_ARGS(&m_depthStencilHeap)), "CreateDescriptorHeap (DSV)"))
    {
        return false;
    }

    m_renderTargetSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_depthStencilSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    m_initialized = true;
    return true;
}

void D3D12ThinDevice::VRelease()
{
    FN("D3D12ThinDevice::VRelease");

    if (!m_initialized)
    {
        return;
    }

    VWaitIdle();

    m_queues.clear();
    m_renderTargetHeap.Reset();
    m_depthStencilHeap.Reset();
    m_device.Reset();
    m_factory.Reset();

    m_initialized = false;
    LOG_TRACE("D3D12ThinDevice released");
}

ThinQueuePtr D3D12ThinDevice::VGetQueue(ThinQueueType type)
{
    FN("D3D12ThinDevice::VGetQueue");

    const int key = (int)type;
    std::unordered_map<int, ThinQueuePtr>::const_iterator existing = m_queues.find(key);
    if (existing != m_queues.end())
    {
        return existing->second;
    }

    D3D12_COMMAND_QUEUE_DESC description = {};
    description.Type = ToCommandListType(type);

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    if (!D3D12Check(m_device->CreateCommandQueue(&description, IID_PPV_ARGS(&queue)), "CreateCommandQueue"))
    {
        return nullptr;
    }

    ThinQueuePtr created = std::make_shared<D3D12ThinQueue>(this, queue, type);
    m_queues[key] = created;
    return created;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12ThinDevice::AllocateRenderTargetView()
{
    FN("D3D12ThinDevice::AllocateRenderTargetView");

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
    if (m_renderTargetsUsed >= MaxRenderTargetViews)
    {
        LOG_ERROR("Out of render target views: the device reserves %u.", MaxRenderTargetViews);
        return handle;
    }

    handle.ptr += (SIZE_T)m_renderTargetsUsed * m_renderTargetSize;
    ++m_renderTargetsUsed;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12ThinDevice::AllocateDepthStencilView()
{
    FN("D3D12ThinDevice::AllocateDepthStencilView");

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
    if (m_depthStencilsUsed >= MaxDepthStencilViews)
    {
        LOG_ERROR("Out of depth stencil views: the device reserves %u.", MaxDepthStencilViews);
        return handle;
    }

    handle.ptr += (SIZE_T)m_depthStencilsUsed * m_depthStencilSize;
    ++m_depthStencilsUsed;
    return handle;
}

void D3D12ThinDevice::VWaitIdle()
{
    FN("D3D12ThinDevice::VWaitIdle");

    // There is no device-wide wait in this API, so every queue that was handed
    // out is drained instead.
    for (std::unordered_map<int, ThinQueuePtr>::iterator it = m_queues.begin(); it != m_queues.end(); ++it)
    {
        if (it->second != nullptr)
        {
            it->second->VWaitIdle();
        }
    }
}

} // namespace bow
