#include <DirectX12ThinDevice/BowD3D12ThinObjects.h>

#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D12ThinSwapchain::D3D12ThinSwapchain(D3D12ThinDevice *device, void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount)
    : m_device(device), m_format(DXGI_FORMAT_B8G8R8A8_UNORM), m_width(width), m_height(height)
{
    FN("D3D12ThinSwapchain::D3D12ThinSwapchain");

    ThinQueuePtr queue = device->VGetQueue(ThinQueueType::Graphics);
    if (queue == nullptr)
    {
        LOG_ERROR("No graphics queue to present from.");
        return;
    }

    // The flip model is what current Windows composes without an extra copy;
    // the older sequential models are emulated on top of it anyway.
    DXGI_SWAP_CHAIN_DESC1 description = {};
    description.Width = width;
    description.Height = height;
    description.Format = m_format;
    description.SampleDesc.Count = 1;
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount = (imageCount < 2) ? 2 : imageCount;
    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    description.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
    ID3D12CommandQueue *commandQueue = static_cast<D3D12ThinQueue *>(queue.get())->GetHandle();

    if (!D3D12Check(device->GetFactory()->CreateSwapChainForHwnd(commandQueue, (HWND)nativeWindowHandle, &description, nullptr, nullptr, &swapchain), "CreateSwapChainForHwnd"))
    {
        return;
    }

    // Alt+Enter is left to the application: letting DXGI take it would change
    // the swapchain behind its back.
    device->GetFactory()->MakeWindowAssociation((HWND)nativeWindowHandle, DXGI_MWA_NO_ALT_ENTER);

    if (!D3D12Check(swapchain.As(&m_swapchain), "IDXGISwapChain3"))
    {
        return;
    }

    CreateImages();
}

D3D12ThinSwapchain::~D3D12ThinSwapchain() { FN("D3D12ThinSwapchain::~D3D12ThinSwapchain"); }

bool D3D12ThinSwapchain::CreateImages()
{
    FN("D3D12ThinSwapchain::CreateImages");

    DXGI_SWAP_CHAIN_DESC1 description = {};
    m_swapchain->GetDesc1(&description);

    m_width = description.Width;
    m_height = description.Height;

    ThinTextureDescription textureDescription;
    textureDescription.width = m_width;
    textureDescription.height = m_height;
    textureDescription.format = D3D12ThinTypes::FromDXGIFormat(m_format);
    textureDescription.renderTarget = true;

    m_images.clear();
    for (UINT i = 0; i < description.BufferCount; ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
        if (!D3D12Check(m_swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer)), "GetBuffer"))
        {
            return false;
        }

        m_images.push_back(std::make_shared<D3D12ThinTexture>(m_device, buffer, textureDescription));
    }

    return true;
}

uint32_t D3D12ThinSwapchain::VAcquireNextImage()
{
    FN("D3D12ThinSwapchain::VAcquireNextImage");

    // Nothing is acquired here: DXGI simply says which buffer is next, and the
    // waiting is done by the caller's fence. Vulkan needs a semaphore for the
    // same thing, which is why the interface has an acquire call at all.
    return (m_swapchain != nullptr) ? m_swapchain->GetCurrentBackBufferIndex() : 0;
}

ThinTexturePtr D3D12ThinSwapchain::VGetImage(uint32_t index) const { return (index < m_images.size()) ? m_images[index] : nullptr; }

ThinFormat D3D12ThinSwapchain::VGetFormat() const { return D3D12ThinTypes::FromDXGIFormat(m_format); }

void D3D12ThinSwapchain::VPresent(bool vsync)
{
    FN("D3D12ThinSwapchain::VPresent");

    if (m_swapchain == nullptr)
    {
        return;
    }

    D3D12Check(m_swapchain->Present(vsync ? 1 : 0, 0), "Present");
}

void D3D12ThinSwapchain::VResize(uint32_t width, uint32_t height)
{
    FN("D3D12ThinSwapchain::VResize");

    if (m_swapchain == nullptr || width == 0 || height == 0)
    {
        return;
    }

    // Every reference to the buffers has to go before they can be resized, and
    // the textures wrapping them hold one each.
    m_device->VWaitIdle();
    m_images.clear();

    if (!D3D12Check(m_swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0), "ResizeBuffers"))
    {
        return;
    }

    CreateImages();
}

} // namespace bow
