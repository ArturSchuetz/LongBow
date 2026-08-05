#include <DirectX12ThinDevice/BowD3D12ThinObjects.h>

#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{
namespace
{

D3D12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE type)
{
    D3D12_HEAP_PROPERTIES properties = {};
    properties.Type = type;
    properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    return properties;
}

} // namespace

// ====================================================================== buffer

D3D12ThinBuffer::D3D12ThinBuffer(D3D12ThinDevice *device, const ThinBufferDescription &description) : m_device(device), m_description(description), m_mapped(nullptr)
{
    FN("D3D12ThinBuffer::D3D12ThinBuffer");

    // Constant buffers are read in 256-byte blocks, and one whose size is not
    // a multiple of that is rejected.
    uint64_t size = description.sizeInBytes;
    if (description.constantBuffer)
    {
        size = (size + 255) & ~static_cast<uint64_t>(255);
    }

    D3D12_RESOURCE_DESC resourceDescription = {};
    resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDescription.Width = size;
    resourceDescription.Height = 1;
    resourceDescription.DepthOrArraySize = 1;
    resourceDescription.MipLevels = 1;
    resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
    resourceDescription.SampleDesc.Count = 1;
    resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (description.storageBuffer)
    {
        resourceDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    // An upload heap is CPU-writable and GPU-readable; a readback heap the
    // other way round; a default heap is device-local and needs a copy. Same
    // trade as Vulkan's host-visible versus device-local memory.
    D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

    if (description.cpuReadable)
    {
        heapType = D3D12_HEAP_TYPE_READBACK;
        initialState = D3D12_RESOURCE_STATE_COPY_DEST;
    }
    else if (description.cpuVisible)
    {
        heapType = D3D12_HEAP_TYPE_UPLOAD;
        // An upload heap resource must stay in GENERIC_READ for its lifetime.
        initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
    }

    const D3D12_HEAP_PROPERTIES heap = HeapProperties(heapType);
    D3D12Check(device->GetHandle()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &resourceDescription, initialState, nullptr, IID_PPV_ARGS(&m_resource)), "CreateCommittedResource (buffer)");
}

D3D12ThinBuffer::~D3D12ThinBuffer()
{
    if (m_mapped != nullptr)
    {
        VUnmap();
    }
}

void *D3D12ThinBuffer::VMap()
{
    FN("D3D12ThinBuffer::VMap");

    if (!m_description.cpuVisible && !m_description.cpuReadable)
    {
        LOG_ERROR("This buffer lives in a default heap and cannot be mapped. Create it with cpuVisible or copy through an upload buffer.");
        return nullptr;
    }

    if (m_mapped == nullptr)
    {
        D3D12_RANGE range = {0, 0};
        m_resource->Map(0, &range, &m_mapped);
    }
    return m_mapped;
}

void D3D12ThinBuffer::VUnmap()
{
    if (m_mapped != nullptr)
    {
        m_resource->Unmap(0, nullptr);
        m_mapped = nullptr;
    }
}

uint64_t D3D12ThinBuffer::VGetDeviceAddress() const { return (m_resource != nullptr) ? m_resource->GetGPUVirtualAddress() : 0; }

// ===================================================================== texture

D3D12ThinTexture::D3D12ThinTexture(D3D12ThinDevice *device, const ThinTextureDescription &description)
    : m_device(device), m_description(description), m_renderTargetView(), m_depthStencilView(), m_hasRenderTargetView(false), m_hasDepthStencilView(false)
{
    FN("D3D12ThinTexture::D3D12ThinTexture");

    D3D12_RESOURCE_DESC resourceDescription = {};
    resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDescription.Width = description.width;
    resourceDescription.Height = description.height;
    resourceDescription.DepthOrArraySize = (UINT16)description.arrayLayers;
    resourceDescription.MipLevels = (UINT16)description.mipLevels;
    resourceDescription.Format = D3D12ThinTypes::ToDXGIFormat(description.format);
    resourceDescription.SampleDesc.Count = description.sampleCount;
    resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    if (description.renderTarget)
    {
        resourceDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }
    if (description.depthStencil)
    {
        resourceDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if (!description.sampled)
        {
            // Denying shader access lets the driver pick a faster layout, and
            // is only safe when nothing samples the texture.
            resourceDescription.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }
    }
    if (description.storage)
    {
        resourceDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    const D3D12_HEAP_PROPERTIES heap = HeapProperties(D3D12_HEAP_TYPE_DEFAULT);
    D3D12Check(device->GetHandle()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_resource)), "CreateCommittedResource (texture)");
}

D3D12ThinTexture::D3D12ThinTexture(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const ThinTextureDescription &description)
    : m_device(device), m_description(description), m_resource(resource), m_renderTargetView(), m_depthStencilView(), m_hasRenderTargetView(false), m_hasDepthStencilView(false)
{
}

D3D12ThinTexture::~D3D12ThinTexture() {}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12ThinTexture::GetRenderTargetView()
{
    FN("D3D12ThinTexture::GetRenderTargetView");

    if (!m_hasRenderTargetView)
    {
        m_renderTargetView = m_device->AllocateRenderTargetView();
        m_device->GetHandle()->CreateRenderTargetView(m_resource.Get(), nullptr, m_renderTargetView);
        m_hasRenderTargetView = true;
    }
    return m_renderTargetView;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12ThinTexture::GetDepthStencilView()
{
    FN("D3D12ThinTexture::GetDepthStencilView");

    if (!m_hasDepthStencilView)
    {
        m_depthStencilView = m_device->AllocateDepthStencilView();
        m_device->GetHandle()->CreateDepthStencilView(m_resource.Get(), nullptr, m_depthStencilView);
        m_hasDepthStencilView = true;
    }
    return m_depthStencilView;
}

// ===================================================================== sampler

D3D12ThinSampler::D3D12ThinSampler(D3D12ThinDevice * /*device*/, const ThinSamplerDescription &description)
{
    FN("D3D12ThinSampler::D3D12ThinSampler");

    auto toAddressMode = [](ThinSamplerDescription::AddressMode mode) {
        switch (mode)
        {
        case ThinSamplerDescription::AddressMode::MirroredRepeat:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case ThinSamplerDescription::AddressMode::ClampToEdge:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case ThinSamplerDescription::AddressMode::ClampToBorder:
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        default:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }
    };

    m_description = {};
    if (description.maxAnisotropy > 1.0f)
    {
        m_description.Filter = D3D12_FILTER_ANISOTROPIC;
    }
    else if (description.minLinear && description.magLinear && description.mipLinear)
    {
        m_description.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    }
    else if (description.minLinear && description.magLinear)
    {
        m_description.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    }
    else
    {
        m_description.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    }

    m_description.AddressU = toAddressMode(description.addressU);
    m_description.AddressV = toAddressMode(description.addressV);
    m_description.AddressW = toAddressMode(description.addressW);
    m_description.MaxAnisotropy = (UINT)description.maxAnisotropy;
    m_description.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    m_description.MinLOD = description.minLod;
    m_description.MaxLOD = description.maxLod;
}

D3D12ThinSampler::~D3D12ThinSampler() {}

// =============================================================== shader module

D3D12ThinShaderModule::D3D12ThinShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage) : m_stage(stage)
{
    FN("D3D12ThinShaderModule::D3D12ThinShaderModule");

    // DXIL is handed to the pipeline as a pointer and a length, so the bytes
    // have to outlive this call.
    m_byteCode.resize(sizeInBytes);
    memcpy(m_byteCode.data(), byteCode, sizeInBytes);
}

D3D12ThinShaderModule::~D3D12ThinShaderModule() {}

D3D12_SHADER_BYTECODE D3D12ThinShaderModule::GetByteCode() const
{
    D3D12_SHADER_BYTECODE byteCode = {};
    byteCode.pShaderBytecode = m_byteCode.data();
    byteCode.BytecodeLength = m_byteCode.size();
    return byteCode;
}

} // namespace bow
