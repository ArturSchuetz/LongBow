#pragma once
#include <DirectX12ThinDevice/BowD3D12ThinCommon.h>

#include <unordered_map>

namespace bow
{

//! DirectX 12 behind the thin interface.
/*!
    The counterpart of VulkanThinDevice, against the same interface. Where the
    two APIs differ the difference is confined to the backend: render targets
    are set per draw rather than in a render pass, a fence is a fence rather
    than a timeline semaphore, and descriptor sets are ranges in a heap.
*/
class D3D12ThinDevice : public IThinDevice
{
  public:
    D3D12ThinDevice();
    ~D3D12ThinDevice();

    bool Initialize(uint32_t deviceHandle);
    void VRelease() override;

    const ThinDeviceCapabilities &VGetCapabilities() const override { return m_capabilities; }

    ThinQueuePtr VGetQueue(ThinQueueType type) override;

    ThinSwapchainPtr VCreateSwapchain(void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount) override;

    ThinCommandPoolPtr VCreateCommandPool(ThinQueueType type) override;
    ThinFencePtr VCreateFence(uint64_t initialValue) override;

    ThinBufferPtr VCreateBuffer(const ThinBufferDescription &description) override;
    ThinTexturePtr VCreateTexture(const ThinTextureDescription &description) override;
    ThinSamplerPtr VCreateSampler(const ThinSamplerDescription &description) override;

    ThinShaderModulePtr VCreateShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char *entryPoint) override;

    ThinPipelineLayoutPtr VCreatePipelineLayout(const ThinPipelineLayoutDescription &description) override;
    ThinPipelinePtr VCreateGraphicsPipeline(const ThinGraphicsPipelineDescription &description) override;
    ThinPipelinePtr VCreateComputePipeline(const ThinComputePipelineDescription &description) override;
    ThinPipelinePtr VCreateRayTracingPipeline(const ThinRayTracingPipelineDescription &description) override;

    ThinDescriptorPoolPtr VCreateDescriptorPool(uint32_t maxSets) override;

    ThinAccelerationStructurePtr VCreateBottomLevelAccelerationStructure(const ThinBufferPtr &vertexBuffer, uint32_t vertexCount, uint32_t vertexStrideInBytes, const ThinBufferPtr &indexBuffer, uint32_t indexCount) override;
    ThinAccelerationStructurePtr VCreateTopLevelAccelerationStructure(const std::vector<ThinAccelerationStructurePtr> &bottomLevel) override;

    void VWaitIdle() override;

    ID3D12Device *GetHandle() const { return m_device.Get(); }
    IDXGIFactory6 *GetFactory() const { return m_factory.Get(); }

    //! Hands out a render target view slot from the backing heap.
    D3D12_CPU_DESCRIPTOR_HANDLE AllocateRenderTargetView();
    D3D12_CPU_DESCRIPTOR_HANDLE AllocateDepthStencilView();

  private:
    D3D12ThinDevice(const D3D12ThinDevice &) = delete;
    D3D12ThinDevice &operator=(const D3D12ThinDevice &) = delete;

    Microsoft::WRL::ComPtr<IDXGIFactory6> m_factory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;

    std::unordered_map<int, ThinQueuePtr> m_queues;

    //! Render target and depth views live in non-shader-visible heaps, so they
    //! are allocated linearly and never freed for the device's lifetime.
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_renderTargetHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_depthStencilHeap;
    uint32_t m_renderTargetSize;
    uint32_t m_depthStencilSize;
    uint32_t m_renderTargetsUsed;
    uint32_t m_depthStencilsUsed;

    ThinDeviceCapabilities m_capabilities;
    bool m_initialized;
};

} // namespace bow
