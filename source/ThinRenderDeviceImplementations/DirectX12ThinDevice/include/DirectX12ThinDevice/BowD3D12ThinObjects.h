#pragma once
#include <DirectX12ThinDevice/BowD3D12ThinCommon.h>

#include <vector>

#include <map>

namespace bow
{

class D3D12ThinDevice;

class D3D12ThinQueue : public IThinQueue
{
  public:
    D3D12ThinQueue(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue, ThinQueueType type);
    ~D3D12ThinQueue();

    ThinQueueType VGetType() const override { return m_type; }

    void VSubmit(const std::vector<ThinCommandListPtr> &commandLists, const ThinFencePtr &waitFence, uint64_t waitValue, const ThinFencePtr &signalFence, uint64_t signalValue) override;
    void VWaitIdle() override;

    ID3D12CommandQueue *GetHandle() const { return m_queue.Get(); }

  private:
    D3D12ThinDevice *m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queue;
    ThinQueueType m_type;

    //! Used only by VWaitIdle, which has no equivalent in this API.
    Microsoft::WRL::ComPtr<ID3D12Fence> m_idleFence;
    uint64_t m_idleValue;
};

//! A fence, which is what a Vulkan timeline semaphore is here.
class D3D12ThinFence : public IThinFence
{
  public:
    D3D12ThinFence(D3D12ThinDevice *device, uint64_t initialValue);
    ~D3D12ThinFence();

    uint64_t VGetCompletedValue() const override;
    void VWait(uint64_t value) override;
    void VSignal(uint64_t value) override;

    ID3D12Fence *GetHandle() const { return m_fence.Get(); }

  private:
    D3D12ThinDevice *m_device;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    HANDLE m_event;
};

class D3D12ThinCommandList : public IThinCommandList
{
  public:
    D3D12ThinCommandList(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, ID3D12CommandAllocator *allocator);
    ~D3D12ThinCommandList();

    void VBegin() override;
    void VEnd() override;

    void VBeginRendering(const ThinRenderingInfo &renderingInfo) override;
    void VEndRendering() override;

    void VSetViewport(const ThinViewport &viewport) override;
    void VSetScissor(const ThinScissor &scissor) override;

    void VBindPipeline(const ThinPipelinePtr &pipeline) override;
    void VBindDescriptorSet(uint32_t setIndex, const ThinDescriptorSetPtr &descriptorSet) override;
    void VSetRootConstants(ThinShaderStage stages, uint32_t offsetInBytes, uint32_t sizeInBytes, const void *data) override;

    void VBindVertexBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes) override;
    void VBindIndexBuffer(const ThinBufferPtr &buffer, ThinFormat indexFormat, uint64_t offsetInBytes) override;

    void VDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    void VDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
    void VDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
    void VTraceRays(uint32_t width, uint32_t height, uint32_t depth) override;

    void VCopyBuffer(const ThinBufferPtr &source, const ThinBufferPtr &destination, uint64_t sourceOffset, uint64_t destinationOffset, uint64_t sizeInBytes) override;
    void VCopyBufferToTexture(const ThinBufferPtr &source, const ThinTexturePtr &destination, uint32_t mipLevel) override;

    void VBarrier(const std::vector<ThinBarrier> &barriers) override;

    void VBeginDebugLabel(const char *name) override;
    void VEndDebugLabel() override;

    ID3D12GraphicsCommandList *GetHandle() const { return m_commandList.Get(); }

  private:
    //! Applies the vertex buffer views once a pipeline has supplied strides.
    void FlushVertexBuffers();

    D3D12ThinDevice *m_device;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ID3D12CommandAllocator *m_allocator;

    //! Topology comes from the pipeline description but is set on the list.
    D3D12_PRIMITIVE_TOPOLOGY m_topology;

    //! Views are collected as they are bound and applied before a draw,
    //! because the stride they need comes from the pipeline.
    std::vector<D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferViews;
    std::vector<uint32_t> m_vertexStrides;
    bool m_vertexBuffersDirty;
};

class D3D12ThinCommandPool : public IThinCommandPool
{
  public:
    D3D12ThinCommandPool(D3D12ThinDevice *device, ThinQueueType type);
    ~D3D12ThinCommandPool();

    ThinCommandListPtr VAllocate() override;
    void VReset() override;

  private:
    D3D12ThinDevice *m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
    D3D12_COMMAND_LIST_TYPE m_type;
};

class D3D12ThinBuffer : public IThinBuffer
{
  public:
    D3D12ThinBuffer(D3D12ThinDevice *device, const ThinBufferDescription &description);
    ~D3D12ThinBuffer();

    const ThinBufferDescription &VGetDescription() const override { return m_description; }

    void *VMap() override;
    void VUnmap() override;
    uint64_t VGetDeviceAddress() const override;

    ID3D12Resource *GetHandle() const { return m_resource.Get(); }
    bool IsReady() const { return m_resource != nullptr; }

  private:
    D3D12ThinDevice *m_device;
    ThinBufferDescription m_description;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
    void *m_mapped;
};

class D3D12ThinTexture : public IThinTexture
{
  public:
    D3D12ThinTexture(D3D12ThinDevice *device, const ThinTextureDescription &description);
    //! Wraps a swapchain buffer, which the swapchain owns.
    D3D12ThinTexture(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const ThinTextureDescription &description);
    ~D3D12ThinTexture();

    const ThinTextureDescription &VGetDescription() const override { return m_description; }

    ID3D12Resource *GetHandle() const { return m_resource.Get(); }
    bool IsReady() const { return m_resource != nullptr; }

    //! Render target view, created on first use.
    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView();
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView();

  private:
    D3D12ThinDevice *m_device;
    ThinTextureDescription m_description;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;

    D3D12_CPU_DESCRIPTOR_HANDLE m_renderTargetView;
    D3D12_CPU_DESCRIPTOR_HANDLE m_depthStencilView;
    bool m_hasRenderTargetView;
    bool m_hasDepthStencilView;
};

class D3D12ThinSampler : public IThinSampler
{
  public:
    D3D12ThinSampler(D3D12ThinDevice *device, const ThinSamplerDescription &description);
    ~D3D12ThinSampler();

    const D3D12_SAMPLER_DESC &GetDescription() const { return m_description; }

  private:
    D3D12_SAMPLER_DESC m_description;
};

//! DXIL bytecode, kept as-is until a pipeline consumes it.
class D3D12ThinShaderModule : public IThinShaderModule
{
  public:
    D3D12ThinShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage);
    ~D3D12ThinShaderModule();

    ThinShaderStage VGetStage() const override { return m_stage; }

    D3D12_SHADER_BYTECODE GetByteCode() const;

  private:
    std::vector<char> m_byteCode;
    ThinShaderStage m_stage;
};

//! A root signature, which is what a Vulkan pipeline layout is here.
class D3D12ThinPipelineLayout : public IThinPipelineLayout
{
  public:
    D3D12ThinPipelineLayout(D3D12ThinDevice *device, const ThinPipelineLayoutDescription &description);
    ~D3D12ThinPipelineLayout();

    const ThinPipelineLayoutDescription &VGetDescription() const override { return m_description; }

    ID3D12RootSignature *GetHandle() const { return m_rootSignature.Get(); }

    //! Root parameter index a descriptor set or root constant range landed on.
    uint32_t GetDescriptorSetRootParameter(uint32_t setIndex) const;
    uint32_t GetRootConstantParameter() const { return m_rootConstantParameter; }

  private:
    D3D12ThinDevice *m_device;
    ThinPipelineLayoutDescription m_description;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

    std::vector<uint32_t> m_setRootParameters;
    uint32_t m_rootConstantParameter;
};

class D3D12ThinPipeline : public IThinPipeline
{
  public:
    D3D12ThinPipeline(D3D12ThinDevice *device, Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline, const ThinPipelineLayoutPtr &layout, D3D12_PRIMITIVE_TOPOLOGY topology, bool compute, const std::vector<uint32_t> &vertexStrides);
    ~D3D12ThinPipeline();

    ThinPipelineLayoutPtr VGetLayout() const override { return m_layout; }

    ID3D12PipelineState *GetHandle() const { return m_pipeline.Get(); }
    D3D12_PRIMITIVE_TOPOLOGY GetTopology() const { return m_topology; }
    bool IsCompute() const { return m_compute; }

    //! Stride per vertex buffer slot.
    /*!
        Vulkan puts the stride in the pipeline; DirectX puts it in the vertex
        buffer view, which is set on the command list. The pipeline therefore
        carries it so that a bind can fill the view in correctly.
    */
    const std::vector<uint32_t> &GetVertexStrides() const { return m_vertexStrides; }

  private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline;
    ThinPipelineLayoutPtr m_layout;
    D3D12_PRIMITIVE_TOPOLOGY m_topology;
    bool m_compute;
    std::vector<uint32_t> m_vertexStrides;
};

//! A range in a descriptor heap, which is what a descriptor set is here.
class D3D12ThinDescriptorSet : public IThinDescriptorSet
{
  public:
    D3D12ThinDescriptorSet(D3D12ThinDevice *device, D3D12_CPU_DESCRIPTOR_HANDLE cpuStart, D3D12_GPU_DESCRIPTOR_HANDLE gpuStart, uint32_t descriptorSize);
    ~D3D12ThinDescriptorSet();

    void VSetBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes, uint64_t sizeInBytes) override;
    void VSetTexture(uint32_t binding, const ThinTexturePtr &texture) override;
    void VSetStorageTexture(uint32_t binding, const ThinTexturePtr &texture) override;
    void VSetSampler(uint32_t binding, const ThinSamplerPtr &sampler) override;
    void VSetAccelerationStructure(uint32_t binding, const ThinAccelerationStructurePtr &accelerationStructure) override;
    void VUpdate() override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_gpuStart; }

  private:
    D3D12_CPU_DESCRIPTOR_HANDLE HandleAt(uint32_t binding) const;

    D3D12ThinDevice *m_device;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;
    uint32_t m_descriptorSize;
};

class D3D12ThinDescriptorPool : public IThinDescriptorPool
{
  public:
    D3D12ThinDescriptorPool(D3D12ThinDevice *device, uint32_t maxSets);
    ~D3D12ThinDescriptorPool();

    ThinDescriptorSetPtr VAllocate(const ThinDescriptorSetLayoutDescription &layout) override;
    void VReset() override;

    ID3D12DescriptorHeap *GetHeap() const { return m_heap.Get(); }

  private:
    D3D12ThinDevice *m_device;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
    uint32_t m_descriptorSize;
    uint32_t m_capacity;
    uint32_t m_used;
};

class D3D12ThinSwapchain : public IThinSwapchain
{
  public:
    D3D12ThinSwapchain(D3D12ThinDevice *device, void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount);
    ~D3D12ThinSwapchain();

    uint32_t VAcquireNextImage() override;
    ThinTexturePtr VGetImage(uint32_t index) const override;
    uint32_t VGetImageCount() const override { return (uint32_t)m_images.size(); }
    ThinFormat VGetFormat() const override;

    uint32_t VGetWidth() const override { return m_width; }
    uint32_t VGetHeight() const override { return m_height; }

    void VPresent(bool vsync) override;
    void VResize(uint32_t width, uint32_t height) override;

    bool IsReady() const { return m_swapchain != nullptr; }

  private:
    bool CreateImages();

    D3D12ThinDevice *m_device;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapchain;
    DXGI_FORMAT m_format;

    std::vector<ThinTexturePtr> m_images;

    uint32_t m_width;
    uint32_t m_height;
};

} // namespace bow
