#include <DirectX12ThinDevice/BowD3D12ThinObjects.h>

#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

// ============================================================= root signature

D3D12ThinPipelineLayout::D3D12ThinPipelineLayout(D3D12ThinDevice *device, const ThinPipelineLayoutDescription &description) : m_device(device), m_description(description), m_rootConstantParameter(UINT32_MAX)
{
    FN("D3D12ThinPipelineLayout::D3D12ThinPipelineLayout");

    // Root constants come first so that their parameter index is fixed at 0,
    // which is what the command list assumes when setting them.
    std::vector<D3D12_ROOT_PARAMETER> parameters;
    std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> ranges;
    ranges.reserve(description.descriptorSets.size());

    if (!description.rootConstants.empty())
    {
        uint32_t totalBytes = 0;
        for (const ThinRootConstantRange &range : description.rootConstants)
        {
            totalBytes += range.sizeInBytes;
        }

        D3D12_ROOT_PARAMETER parameter = {};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        parameter.Constants.Num32BitValues = totalBytes / 4;
        parameter.ShaderVisibility = D3D12ThinTypes::ToShaderVisibility(description.rootConstants[0].stages);

        m_rootConstantParameter = (uint32_t)parameters.size();
        parameters.push_back(parameter);
    }

    for (const ThinDescriptorSetLayoutDescription &set : description.descriptorSets)
    {
        std::vector<D3D12_DESCRIPTOR_RANGE> setRanges;
        setRanges.reserve(set.bindings.size());

        for (const ThinDescriptorBinding &binding : set.bindings)
        {
            D3D12_DESCRIPTOR_RANGE range = {};
            range.RangeType = D3D12ThinTypes::ToDescriptorRangeType(binding.type);
            range.NumDescriptors = binding.count;
            range.BaseShaderRegister = binding.binding;
            range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            setRanges.push_back(range);
        }

        if (setRanges.empty())
        {
            m_setRootParameters.push_back(UINT32_MAX);
            continue;
        }

        ranges.push_back(setRanges);

        D3D12_ROOT_PARAMETER parameter = {};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.NumDescriptorRanges = (UINT)ranges.back().size();
        parameter.DescriptorTable.pDescriptorRanges = ranges.back().data();
        parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        m_setRootParameters.push_back((uint32_t)parameters.size());
        parameters.push_back(parameter);
    }

    D3D12_ROOT_SIGNATURE_DESC signature = {};
    signature.NumParameters = (UINT)parameters.size();
    signature.pParameters = parameters.data();
    signature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> serialized;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    if (FAILED(D3D12SerializeRootSignature(&signature, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &errors)))
    {
        if (errors != nullptr)
        {
            LOG_ERROR("Could not serialise the root signature: %s", static_cast<const char *>(errors->GetBufferPointer()));
        }
        return;
    }

    D3D12Check(device->GetHandle()->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)), "CreateRootSignature");
}

D3D12ThinPipelineLayout::~D3D12ThinPipelineLayout() {}

uint32_t D3D12ThinPipelineLayout::GetDescriptorSetRootParameter(uint32_t setIndex) const { return (setIndex < m_setRootParameters.size()) ? m_setRootParameters[setIndex] : UINT32_MAX; }

// ==================================================================== pipeline

D3D12ThinPipeline::D3D12ThinPipeline(D3D12ThinDevice * /*device*/, Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline, const ThinPipelineLayoutPtr &layout, D3D12_PRIMITIVE_TOPOLOGY topology, bool compute,
                                     const std::vector<uint32_t> &vertexStrides)
    : m_pipeline(pipeline), m_layout(layout), m_topology(topology), m_compute(compute), m_vertexStrides(vertexStrides)
{
}

D3D12ThinPipeline::~D3D12ThinPipeline() {}

// ============================================================== descriptor set

D3D12ThinDescriptorSet::D3D12ThinDescriptorSet(D3D12ThinDevice *device, D3D12_CPU_DESCRIPTOR_HANDLE cpuStart, D3D12_GPU_DESCRIPTOR_HANDLE gpuStart, uint32_t descriptorSize)
    : m_device(device), m_cpuStart(cpuStart), m_gpuStart(gpuStart), m_descriptorSize(descriptorSize)
{
}

D3D12ThinDescriptorSet::~D3D12ThinDescriptorSet() {}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12ThinDescriptorSet::HandleAt(uint32_t binding) const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStart;
    handle.ptr += (SIZE_T)binding * m_descriptorSize;
    return handle;
}

void D3D12ThinDescriptorSet::VSetBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes, uint64_t sizeInBytes)
{
    FN("D3D12ThinDescriptorSet::VSetBuffer");

    D3D12ThinBuffer *d3dBuffer = static_cast<D3D12ThinBuffer *>(buffer.get());
    if (d3dBuffer == nullptr)
    {
        return;
    }

    // Descriptors are written straight into the heap here; there is nothing to
    // batch, which is why VUpdate has nothing to do on this backend.
    if (d3dBuffer->VGetDescription().storageBuffer)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC view = {};
        view.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        view.Format = DXGI_FORMAT_R32_TYPELESS;
        view.Buffer.FirstElement = offsetInBytes / 4;
        view.Buffer.NumElements = (UINT)((sizeInBytes > 0 ? sizeInBytes : d3dBuffer->VGetDescription().sizeInBytes) / 4);
        view.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

        m_device->GetHandle()->CreateUnorderedAccessView(d3dBuffer->GetHandle(), nullptr, &view, HandleAt(binding));
        return;
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC view = {};
    view.BufferLocation = d3dBuffer->GetHandle()->GetGPUVirtualAddress() + offsetInBytes;
    // A constant buffer view has to be a multiple of 256 bytes.
    const uint64_t size = (sizeInBytes > 0) ? sizeInBytes : d3dBuffer->VGetDescription().sizeInBytes;
    view.SizeInBytes = (UINT)((size + 255) & ~static_cast<uint64_t>(255));

    m_device->GetHandle()->CreateConstantBufferView(&view, HandleAt(binding));
}

void D3D12ThinDescriptorSet::VSetTexture(uint32_t binding, const ThinTexturePtr &texture)
{
    FN("D3D12ThinDescriptorSet::VSetTexture");

    D3D12ThinTexture *d3dTexture = static_cast<D3D12ThinTexture *>(texture.get());
    if (d3dTexture == nullptr)
    {
        return;
    }

    m_device->GetHandle()->CreateShaderResourceView(d3dTexture->GetHandle(), nullptr, HandleAt(binding));
}

void D3D12ThinDescriptorSet::VSetStorageTexture(uint32_t binding, const ThinTexturePtr &texture)
{
    FN("D3D12ThinDescriptorSet::VSetStorageTexture");

    D3D12ThinTexture *d3dTexture = static_cast<D3D12ThinTexture *>(texture.get());
    if (d3dTexture == nullptr)
    {
        return;
    }

    m_device->GetHandle()->CreateUnorderedAccessView(d3dTexture->GetHandle(), nullptr, nullptr, HandleAt(binding));
}

void D3D12ThinDescriptorSet::VSetSampler(uint32_t /*binding*/, const ThinSamplerPtr & /*sampler*/)
{
    FN("D3D12ThinDescriptorSet::VSetSampler");

    // Samplers live in their own heap type on this API and cannot share the
    // one used for the other descriptors, so a separate pool is needed before
    // this can work.
    LOG_ERROR("Separate sampler descriptors are not wired up in the thin DirectX 12 backend yet.");
}

void D3D12ThinDescriptorSet::VSetAccelerationStructure(uint32_t, const ThinAccelerationStructurePtr &)
{
    FN("D3D12ThinDescriptorSet::VSetAccelerationStructure");

    LOG_ERROR("Ray tracing is not wired up in the thin DirectX 12 backend yet.");
}

void D3D12ThinDescriptorSet::VUpdate()
{
    // Nothing to do: the writes above went straight into the heap. Vulkan
    // batches them instead, which is why the interface has this call at all.
}

// ============================================================= descriptor pool

D3D12ThinDescriptorPool::D3D12ThinDescriptorPool(D3D12ThinDevice *device, uint32_t maxSets) : m_device(device), m_descriptorSize(0), m_capacity(maxSets * 8), m_used(0)
{
    FN("D3D12ThinDescriptorPool::D3D12ThinDescriptorPool");

    D3D12_DESCRIPTOR_HEAP_DESC description = {};
    description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    description.NumDescriptors = m_capacity;
    description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (D3D12Check(device->GetHandle()->CreateDescriptorHeap(&description, IID_PPV_ARGS(&m_heap)), "CreateDescriptorHeap (CBV/SRV/UAV)"))
    {
        m_descriptorSize = device->GetHandle()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}

D3D12ThinDescriptorPool::~D3D12ThinDescriptorPool() {}

ThinDescriptorSetPtr D3D12ThinDescriptorPool::VAllocate(const ThinDescriptorSetLayoutDescription &layout)
{
    FN("D3D12ThinDescriptorPool::VAllocate");

    const uint32_t needed = (uint32_t)layout.bindings.size();
    if (m_used + needed > m_capacity)
    {
        LOG_ERROR("The descriptor heap is full: %u of %u used and %u more requested.", m_used, m_capacity, needed);
        return nullptr;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpu = m_heap->GetCPUDescriptorHandleForHeapStart();
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = m_heap->GetGPUDescriptorHandleForHeapStart();
    cpu.ptr += (SIZE_T)m_used * m_descriptorSize;
    gpu.ptr += (UINT64)m_used * m_descriptorSize;

    m_used += needed;

    return std::make_shared<D3D12ThinDescriptorSet>(m_device, cpu, gpu, m_descriptorSize);
}

void D3D12ThinDescriptorPool::VReset()
{
    FN("D3D12ThinDescriptorPool::VReset");

    m_used = 0;
}

} // namespace bow
