#include <DirectX12ThinDevice/BowD3D12ThinObjects.h>

#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

// ============================================================ queue and fence

D3D12ThinQueue::D3D12ThinQueue(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue, ThinQueueType type) : m_device(device), m_queue(queue), m_type(type), m_idleValue(0)
{
    device->GetHandle()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_idleFence));
}

D3D12ThinQueue::~D3D12ThinQueue() {}

void D3D12ThinQueue::VSubmit(const std::vector<ThinCommandListPtr> &commandLists, const ThinFencePtr &waitFence, uint64_t waitValue, const ThinFencePtr &signalFence, uint64_t signalValue)
{
    FN("D3D12ThinQueue::VSubmit");

    // A wait is queued rather than performed, which is what Vulkan's semaphore
    // wait does too; the ordering therefore matches.
    if (waitFence != nullptr)
    {
        m_queue->Wait(static_cast<D3D12ThinFence *>(waitFence.get())->GetHandle(), waitValue);
    }

    std::vector<ID3D12CommandList *> lists;
    lists.reserve(commandLists.size());
    for (const ThinCommandListPtr &list : commandLists)
    {
        D3D12ThinCommandList *d3dList = static_cast<D3D12ThinCommandList *>(list.get());
        if (d3dList != nullptr)
        {
            lists.push_back(d3dList->GetHandle());
        }
    }

    if (!lists.empty())
    {
        m_queue->ExecuteCommandLists((UINT)lists.size(), lists.data());
    }

    if (signalFence != nullptr)
    {
        m_queue->Signal(static_cast<D3D12ThinFence *>(signalFence.get())->GetHandle(), signalValue);
    }
}

void D3D12ThinQueue::VWaitIdle()
{
    FN("D3D12ThinQueue::VWaitIdle");

    if (m_idleFence == nullptr)
    {
        return;
    }

    ++m_idleValue;
    m_queue->Signal(m_idleFence.Get(), m_idleValue);

    if (m_idleFence->GetCompletedValue() < m_idleValue)
    {
        HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        m_idleFence->SetEventOnCompletion(m_idleValue, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

D3D12ThinFence::D3D12ThinFence(D3D12ThinDevice *device, uint64_t initialValue) : m_device(device), m_event(nullptr)
{
    FN("D3D12ThinFence::D3D12ThinFence");

    D3D12Check(device->GetHandle()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)), "CreateFence");
    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

D3D12ThinFence::~D3D12ThinFence()
{
    if (m_event != nullptr)
    {
        CloseHandle(m_event);
    }
}

uint64_t D3D12ThinFence::VGetCompletedValue() const { return (m_fence != nullptr) ? m_fence->GetCompletedValue() : 0; }

void D3D12ThinFence::VWait(uint64_t value)
{
    FN("D3D12ThinFence::VWait");

    if (m_fence == nullptr || m_fence->GetCompletedValue() >= value)
    {
        return;
    }

    m_fence->SetEventOnCompletion(value, m_event);
    WaitForSingleObject(m_event, INFINITE);
}

void D3D12ThinFence::VSignal(uint64_t value)
{
    FN("D3D12ThinFence::VSignal");

    if (m_fence != nullptr)
    {
        m_fence->Signal(value);
    }
}

// ================================================================ command list

D3D12ThinCommandList::D3D12ThinCommandList(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, ID3D12CommandAllocator *allocator)
    : m_device(device), m_commandList(commandList), m_allocator(allocator), m_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), m_vertexBuffersDirty(false)
{
}

D3D12ThinCommandList::~D3D12ThinCommandList() {}

void D3D12ThinCommandList::VBegin()
{
    // A list is created open, so it is reset onto its allocator rather than
    // begun. Vulkan makes the same distinction with vkBeginCommandBuffer.
    m_commandList->Reset(m_allocator, nullptr);
}

void D3D12ThinCommandList::VEnd() { m_commandList->Close(); }

void D3D12ThinCommandList::VBeginRendering(const ThinRenderingInfo &renderingInfo)
{
    FN("D3D12ThinCommandList::VBeginRendering");

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargets;
    renderTargets.reserve(renderingInfo.colorAttachments.size());

    for (const ThinColorAttachment &attachment : renderingInfo.colorAttachments)
    {
        D3D12ThinTexture *texture = static_cast<D3D12ThinTexture *>(attachment.texture.get());
        if (texture == nullptr)
        {
            continue;
        }

        const D3D12_CPU_DESCRIPTOR_HANDLE view = texture->GetRenderTargetView();
        renderTargets.push_back(view);

        if (attachment.loadOp == ThinLoadOp::Clear)
        {
            m_commandList->ClearRenderTargetView(view, attachment.clearColor, 0, nullptr);
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE depthView = {};
    const bool hasDepth = (renderingInfo.depthAttachment.texture != nullptr);
    if (hasDepth)
    {
        D3D12ThinTexture *texture = static_cast<D3D12ThinTexture *>(renderingInfo.depthAttachment.texture.get());
        depthView = texture->GetDepthStencilView();

        if (renderingInfo.depthAttachment.loadOp == ThinLoadOp::Clear)
        {
            m_commandList->ClearDepthStencilView(depthView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, renderingInfo.depthAttachment.clearDepth, renderingInfo.depthAttachment.clearStencil, 0, nullptr);
        }
    }

    // No render pass object: the targets are simply set, which is what dynamic
    // rendering gives Vulkan and what this API has always done.
    m_commandList->OMSetRenderTargets((UINT)renderTargets.size(), renderTargets.data(), FALSE, hasDepth ? &depthView : nullptr);
}

void D3D12ThinCommandList::VEndRendering()
{
    // Nothing to end: there is no render pass to close.
}

void D3D12ThinCommandList::VSetViewport(const ThinViewport &viewport)
{
    D3D12_VIEWPORT d3dViewport = {};
    d3dViewport.TopLeftX = viewport.x;
    d3dViewport.TopLeftY = viewport.y;
    d3dViewport.Width = viewport.width;
    d3dViewport.Height = viewport.height;
    d3dViewport.MinDepth = viewport.minDepth;
    d3dViewport.MaxDepth = viewport.maxDepth;

    m_commandList->RSSetViewports(1, &d3dViewport);
}

void D3D12ThinCommandList::VSetScissor(const ThinScissor &scissor)
{
    D3D12_RECT rect = {};
    rect.left = scissor.x;
    rect.top = scissor.y;
    rect.right = scissor.x + (LONG)scissor.width;
    rect.bottom = scissor.y + (LONG)scissor.height;

    m_commandList->RSSetScissorRects(1, &rect);
}

void D3D12ThinCommandList::VBindPipeline(const ThinPipelinePtr &pipeline)
{
    FN("D3D12ThinCommandList::VBindPipeline");

    D3D12ThinPipeline *d3dPipeline = static_cast<D3D12ThinPipeline *>(pipeline.get());
    if (d3dPipeline == nullptr)
    {
        return;
    }

    m_commandList->SetPipelineState(d3dPipeline->GetHandle());

    ID3D12RootSignature *rootSignature = static_cast<D3D12ThinPipelineLayout *>(d3dPipeline->VGetLayout().get())->GetHandle();
    if (d3dPipeline->IsCompute())
    {
        m_commandList->SetComputeRootSignature(rootSignature);
    }
    else
    {
        m_commandList->SetGraphicsRootSignature(rootSignature);
        // Topology is part of the pipeline state on Vulkan but set on the list
        // here, so it is carried through the pipeline object.
        m_topology = d3dPipeline->GetTopology();
        m_commandList->IASetPrimitiveTopology(m_topology);

        // The strides the views need only become known once a pipeline is
        // bound, so anything bound before this has to be revisited.
        m_vertexStrides = d3dPipeline->GetVertexStrides();
        m_vertexBuffersDirty = true;
    }
}

void D3D12ThinCommandList::VBindDescriptorSet(uint32_t setIndex, const ThinDescriptorSetPtr &descriptorSet)
{
    FN("D3D12ThinCommandList::VBindDescriptorSet");

    D3D12ThinDescriptorSet *set = static_cast<D3D12ThinDescriptorSet *>(descriptorSet.get());
    if (set == nullptr)
    {
        return;
    }

    m_commandList->SetGraphicsRootDescriptorTable(setIndex, set->GetGpuHandle());
}

void D3D12ThinCommandList::VSetRootConstants(ThinShaderStage /*stages*/, uint32_t offsetInBytes, uint32_t sizeInBytes, const void *data)
{
    FN("D3D12ThinCommandList::VSetRootConstants");

    // Root constants are counted in 32-bit values here, and there is no stage
    // mask: the range's visibility was decided when the root signature was
    // built.
    const UINT count = sizeInBytes / 4;
    const UINT offset = offsetInBytes / 4;

    // The root constant range is always the last parameter, which is how
    // D3D12ThinPipelineLayout lays a signature out.
    m_commandList->SetGraphicsRoot32BitConstants(0, count, data, offset);
}

void D3D12ThinCommandList::VBindVertexBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes)
{
    D3D12ThinBuffer *d3dBuffer = static_cast<D3D12ThinBuffer *>(buffer.get());
    if (d3dBuffer == nullptr)
    {
        return;
    }

    if (m_vertexBufferViews.size() <= binding)
    {
        m_vertexBufferViews.resize(binding + 1);
    }

    D3D12_VERTEX_BUFFER_VIEW view = {};
    view.BufferLocation = d3dBuffer->GetHandle()->GetGPUVirtualAddress() + offsetInBytes;
    view.SizeInBytes = (UINT)(d3dBuffer->VGetDescription().sizeInBytes - offsetInBytes);
    // The stride is filled in by FlushVertexBuffers, because it comes from the
    // pipeline rather than from the buffer.
    view.StrideInBytes = 0;

    m_vertexBufferViews[binding] = view;
    m_vertexBuffersDirty = true;
}

void D3D12ThinCommandList::VBindIndexBuffer(const ThinBufferPtr &buffer, ThinFormat indexFormat, uint64_t offsetInBytes)
{
    D3D12ThinBuffer *d3dBuffer = static_cast<D3D12ThinBuffer *>(buffer.get());
    if (d3dBuffer == nullptr)
    {
        return;
    }

    D3D12_INDEX_BUFFER_VIEW view = {};
    view.BufferLocation = d3dBuffer->GetHandle()->GetGPUVirtualAddress() + offsetInBytes;
    view.SizeInBytes = (UINT)(d3dBuffer->VGetDescription().sizeInBytes - offsetInBytes);
    view.Format = (indexFormat == ThinFormat::R16Uint) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

    m_commandList->IASetIndexBuffer(&view);
}

void D3D12ThinCommandList::FlushVertexBuffers()
{
    FN("D3D12ThinCommandList::FlushVertexBuffers");

    if (!m_vertexBuffersDirty || m_vertexBufferViews.empty())
    {
        return;
    }

    for (size_t i = 0; i < m_vertexBufferViews.size(); ++i)
    {
        m_vertexBufferViews[i].StrideInBytes = (i < m_vertexStrides.size()) ? m_vertexStrides[i] : 0;
    }

    m_commandList->IASetVertexBuffers(0, (UINT)m_vertexBufferViews.size(), m_vertexBufferViews.data());
    m_vertexBuffersDirty = false;
}

void D3D12ThinCommandList::VDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    FlushVertexBuffers();
    m_commandList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}

void D3D12ThinCommandList::VDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    FlushVertexBuffers();
    m_commandList->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void D3D12ThinCommandList::VDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { m_commandList->Dispatch(groupCountX, groupCountY, groupCountZ); }

void D3D12ThinCommandList::VTraceRays(uint32_t, uint32_t, uint32_t)
{
    FN("D3D12ThinCommandList::VTraceRays");

    LOG_ERROR("Ray tracing is not wired up in the thin DirectX 12 backend yet.");
}

void D3D12ThinCommandList::VCopyBuffer(const ThinBufferPtr &source, const ThinBufferPtr &destination, uint64_t sourceOffset, uint64_t destinationOffset, uint64_t sizeInBytes)
{
    m_commandList->CopyBufferRegion(static_cast<D3D12ThinBuffer *>(destination.get())->GetHandle(), destinationOffset, static_cast<D3D12ThinBuffer *>(source.get())->GetHandle(), sourceOffset, sizeInBytes);
}

void D3D12ThinCommandList::VCopyBufferToTexture(const ThinBufferPtr &source, const ThinTexturePtr &destination, uint32_t mipLevel)
{
    FN("D3D12ThinCommandList::VCopyBufferToTexture");

    D3D12ThinTexture *texture = static_cast<D3D12ThinTexture *>(destination.get());
    D3D12ThinBuffer *buffer = static_cast<D3D12ThinBuffer *>(source.get());
    if (texture == nullptr || buffer == nullptr)
    {
        return;
    }

    D3D12_RESOURCE_DESC resourceDescription = texture->GetHandle()->GetDesc();

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
    m_device->GetHandle()->GetCopyableFootprints(&resourceDescription, mipLevel, 1, 0, &footprint, nullptr, nullptr, nullptr);

    D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
    sourceLocation.pResource = buffer->GetHandle();
    sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    sourceLocation.PlacedFootprint = footprint;

    D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
    destinationLocation.pResource = texture->GetHandle();
    destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    destinationLocation.SubresourceIndex = mipLevel;

    m_commandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
}

void D3D12ThinCommandList::VBarrier(const std::vector<ThinBarrier> &barriers)
{
    FN("D3D12ThinCommandList::VBarrier");

    std::vector<D3D12_RESOURCE_BARRIER> d3dBarriers;
    d3dBarriers.reserve(barriers.size());

    for (const ThinBarrier &barrier : barriers)
    {
        ID3D12Resource *resource = nullptr;
        if (barrier.texture != nullptr)
        {
            resource = static_cast<D3D12ThinTexture *>(barrier.texture.get())->GetHandle();
        }
        else if (barrier.buffer != nullptr)
        {
            resource = static_cast<D3D12ThinBuffer *>(barrier.buffer.get())->GetHandle();
        }

        if (resource == nullptr)
        {
            continue;
        }

        const D3D12_RESOURCE_STATES before = D3D12ThinTypes::ToResourceStates(barrier.before);
        const D3D12_RESOURCE_STATES after = D3D12ThinTypes::ToResourceStates(barrier.after);

        // A transition to the state it is already in is rejected outright,
        // where Vulkan would treat it as a no-op.
        if (before == after)
        {
            continue;
        }

        D3D12_RESOURCE_BARRIER d3dBarrier = {};
        d3dBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3dBarrier.Transition.pResource = resource;
        d3dBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        d3dBarrier.Transition.StateBefore = before;
        d3dBarrier.Transition.StateAfter = after;

        d3dBarriers.push_back(d3dBarrier);
    }

    if (!d3dBarriers.empty())
    {
        m_commandList->ResourceBarrier((UINT)d3dBarriers.size(), d3dBarriers.data());
    }
}

void D3D12ThinCommandList::VBeginDebugLabel(const char * /*name*/)
{
    // PIX markers need the WinPixEventRuntime, which is not a dependency here.
}

void D3D12ThinCommandList::VEndDebugLabel() {}

// ================================================================ command pool

D3D12ThinCommandPool::D3D12ThinCommandPool(D3D12ThinDevice *device, ThinQueueType type) : m_device(device)
{
    FN("D3D12ThinCommandPool::D3D12ThinCommandPool");

    switch (type)
    {
    case ThinQueueType::Compute:
        m_type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    case ThinQueueType::Transfer:
        m_type = D3D12_COMMAND_LIST_TYPE_COPY;
        break;
    default:
        m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    }

    D3D12Check(device->GetHandle()->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_allocator)), "CreateCommandAllocator");
}

D3D12ThinCommandPool::~D3D12ThinCommandPool() {}

ThinCommandListPtr D3D12ThinCommandPool::VAllocate()
{
    FN("D3D12ThinCommandPool::VAllocate");

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    if (!D3D12Check(m_device->GetHandle()->CreateCommandList(0, m_type, m_allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)), "CreateCommandList"))
    {
        return nullptr;
    }

    // A new list is open; closing it here means VBegin can reset it like every
    // other list, so the interface behaves the same on both backends.
    commandList->Close();

    return std::make_shared<D3D12ThinCommandList>(m_device, commandList, m_allocator.Get());
}

void D3D12ThinCommandPool::VReset()
{
    FN("D3D12ThinCommandPool::VReset");

    m_allocator->Reset();
}

} // namespace bow
