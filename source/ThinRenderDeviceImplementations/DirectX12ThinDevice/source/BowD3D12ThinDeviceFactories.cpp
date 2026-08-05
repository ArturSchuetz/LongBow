#include <DirectX12ThinDevice/BowD3D12ThinDevice.h>

#include <DirectX12ThinDevice/BowD3D12ThinObjects.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

ThinSwapchainPtr D3D12ThinDevice::VCreateSwapchain(void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount)
{
    FN("D3D12ThinDevice::VCreateSwapchain");

    std::shared_ptr<D3D12ThinSwapchain> swapchain = std::make_shared<D3D12ThinSwapchain>(this, nativeWindowHandle, width, height, imageCount);
    return swapchain->IsReady() ? swapchain : nullptr;
}

ThinCommandPoolPtr D3D12ThinDevice::VCreateCommandPool(ThinQueueType type)
{
    FN("D3D12ThinDevice::VCreateCommandPool");

    return std::make_shared<D3D12ThinCommandPool>(this, type);
}

ThinFencePtr D3D12ThinDevice::VCreateFence(uint64_t initialValue)
{
    FN("D3D12ThinDevice::VCreateFence");

    return std::make_shared<D3D12ThinFence>(this, initialValue);
}

ThinBufferPtr D3D12ThinDevice::VCreateBuffer(const ThinBufferDescription &description)
{
    FN("D3D12ThinDevice::VCreateBuffer");

    std::shared_ptr<D3D12ThinBuffer> buffer = std::make_shared<D3D12ThinBuffer>(this, description);
    return buffer->IsReady() ? buffer : nullptr;
}

ThinTexturePtr D3D12ThinDevice::VCreateTexture(const ThinTextureDescription &description)
{
    FN("D3D12ThinDevice::VCreateTexture");

    std::shared_ptr<D3D12ThinTexture> texture = std::make_shared<D3D12ThinTexture>(this, description);
    return texture->IsReady() ? texture : nullptr;
}

ThinSamplerPtr D3D12ThinDevice::VCreateSampler(const ThinSamplerDescription &description)
{
    FN("D3D12ThinDevice::VCreateSampler");

    return std::make_shared<D3D12ThinSampler>(this, description);
}

ThinShaderModulePtr D3D12ThinDevice::VCreateShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char * /*entryPoint*/)
{
    FN("D3D12ThinDevice::VCreateShaderModule");

    if (byteCode == nullptr || sizeInBytes == 0)
    {
        LOG_ERROR("No bytecode given for the shader module.");
        return nullptr;
    }

    // The entry point is baked into DXIL at compile time, unlike SPIR-V where
    // it is named when the pipeline is built. Nothing to record here.
    return std::make_shared<D3D12ThinShaderModule>(byteCode, sizeInBytes, stage);
}

ThinPipelineLayoutPtr D3D12ThinDevice::VCreatePipelineLayout(const ThinPipelineLayoutDescription &description)
{
    FN("D3D12ThinDevice::VCreatePipelineLayout");

    return std::make_shared<D3D12ThinPipelineLayout>(this, description);
}

ThinPipelinePtr D3D12ThinDevice::VCreateGraphicsPipeline(const ThinGraphicsPipelineDescription &description)
{
    FN("D3D12ThinDevice::VCreateGraphicsPipeline");

    if (description.layout == nullptr || description.vertexShader == nullptr || description.fragmentShader == nullptr)
    {
        LOG_ERROR("A graphics pipeline needs a layout, a vertex shader and a fragment shader.");
        return nullptr;
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
    inputElements.reserve(description.vertexAttributes.size());
    for (const ThinVertexAttribute &attribute : description.vertexAttributes)
    {
        D3D12_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = attribute.semanticName.c_str();
        element.SemanticIndex = attribute.semanticIndex;
        element.Format = D3D12ThinTypes::ToDXGIFormat(attribute.format);
        element.InputSlot = attribute.bufferBinding;
        element.AlignedByteOffset = attribute.offsetInBytes;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        inputElements.push_back(element);
    }

    std::vector<uint32_t> vertexStrides;
    for (const ThinVertexBufferBinding &binding : description.vertexBuffers)
    {
        if (vertexStrides.size() <= binding.binding)
        {
            vertexStrides.resize(binding.binding + 1, 0);
        }
        vertexStrides[binding.binding] = binding.strideInBytes;

        for (D3D12_INPUT_ELEMENT_DESC &element : inputElements)
        {
            if (element.InputSlot == binding.binding && binding.perInstance)
            {
                element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                element.InstanceDataStepRate = 1;
            }
        }
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDescription = {};
    pipelineDescription.pRootSignature = static_cast<D3D12ThinPipelineLayout *>(description.layout.get())->GetHandle();
    pipelineDescription.VS = static_cast<D3D12ThinShaderModule *>(description.vertexShader.get())->GetByteCode();
    pipelineDescription.PS = static_cast<D3D12ThinShaderModule *>(description.fragmentShader.get())->GetByteCode();
    if (description.geometryShader != nullptr)
    {
        pipelineDescription.GS = static_cast<D3D12ThinShaderModule *>(description.geometryShader.get())->GetByteCode();
    }

    pipelineDescription.InputLayout.NumElements = (UINT)inputElements.size();
    pipelineDescription.InputLayout.pInputElementDescs = inputElements.data();

    pipelineDescription.RasterizerState.FillMode = description.wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
    pipelineDescription.RasterizerState.CullMode = D3D12ThinTypes::ToCullMode(description.cullMode);
    pipelineDescription.RasterizerState.FrontCounterClockwise = description.frontFaceCounterClockwise ? TRUE : FALSE;
    pipelineDescription.RasterizerState.DepthClipEnable = TRUE;

    pipelineDescription.DepthStencilState.DepthEnable = description.depthTestEnabled ? TRUE : FALSE;
    pipelineDescription.DepthStencilState.DepthWriteMask = description.depthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    pipelineDescription.DepthStencilState.DepthFunc = D3D12ThinTypes::ToComparisonFunc(description.depthCompareOp);

    pipelineDescription.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    for (size_t i = 0; i < description.colorAttachmentFormats.size() && i < 8; ++i)
    {
        pipelineDescription.RTVFormats[i] = D3D12ThinTypes::ToDXGIFormat(description.colorAttachmentFormats[i]);
        pipelineDescription.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        if (i < description.colorAttachmentBlends.size() && description.colorAttachmentBlends[i].enabled)
        {
            const ThinColorAttachmentBlend &blend = description.colorAttachmentBlends[i];
            pipelineDescription.BlendState.RenderTarget[i].BlendEnable = TRUE;
            pipelineDescription.BlendState.RenderTarget[i].SrcBlend = D3D12ThinTypes::ToBlend(blend.sourceColor);
            pipelineDescription.BlendState.RenderTarget[i].DestBlend = D3D12ThinTypes::ToBlend(blend.destinationColor);
            pipelineDescription.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
            pipelineDescription.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12ThinTypes::ToBlend(blend.sourceAlpha);
            pipelineDescription.BlendState.RenderTarget[i].DestBlendAlpha = D3D12ThinTypes::ToBlend(blend.destinationAlpha);
            pipelineDescription.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        }
    }

    pipelineDescription.NumRenderTargets = (UINT)description.colorAttachmentFormats.size();
    pipelineDescription.DSVFormat = D3D12ThinTypes::ToDXGIFormat(description.depthAttachmentFormat);
    pipelineDescription.PrimitiveTopologyType = D3D12ThinTypes::ToTopologyType(description.topology);
    pipelineDescription.SampleMask = UINT_MAX;
    pipelineDescription.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
    if (!D3D12Check(m_device->CreateGraphicsPipelineState(&pipelineDescription, IID_PPV_ARGS(&pipeline)), "CreateGraphicsPipelineState"))
    {
        return nullptr;
    }

    return std::make_shared<D3D12ThinPipeline>(this, pipeline, description.layout, D3D12ThinTypes::ToTopology(description.topology), false, vertexStrides);
}

ThinPipelinePtr D3D12ThinDevice::VCreateComputePipeline(const ThinComputePipelineDescription &description)
{
    FN("D3D12ThinDevice::VCreateComputePipeline");

    if (description.layout == nullptr || description.computeShader == nullptr)
    {
        LOG_ERROR("A compute pipeline needs a layout and a compute shader.");
        return nullptr;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineDescription = {};
    pipelineDescription.pRootSignature = static_cast<D3D12ThinPipelineLayout *>(description.layout.get())->GetHandle();
    pipelineDescription.CS = static_cast<D3D12ThinShaderModule *>(description.computeShader.get())->GetByteCode();

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
    if (!D3D12Check(m_device->CreateComputePipelineState(&pipelineDescription, IID_PPV_ARGS(&pipeline)), "CreateComputePipelineState"))
    {
        return nullptr;
    }

    return std::make_shared<D3D12ThinPipeline>(this, pipeline, description.layout, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, true, std::vector<uint32_t>());
}

ThinPipelinePtr D3D12ThinDevice::VCreateRayTracingPipeline(const ThinRayTracingPipelineDescription &)
{
    FN("D3D12ThinDevice::VCreateRayTracingPipeline");

    LOG_ERROR("Ray tracing pipelines are not wired up in the thin DirectX 12 backend yet.");
    return nullptr;
}

ThinDescriptorPoolPtr D3D12ThinDevice::VCreateDescriptorPool(uint32_t maxSets)
{
    FN("D3D12ThinDevice::VCreateDescriptorPool");

    return std::make_shared<D3D12ThinDescriptorPool>(this, maxSets);
}

ThinAccelerationStructurePtr D3D12ThinDevice::VCreateBottomLevelAccelerationStructure(const ThinBufferPtr &, uint32_t, uint32_t, const ThinBufferPtr &, uint32_t)
{
    FN("D3D12ThinDevice::VCreateBottomLevelAccelerationStructure");

    LOG_ERROR("Acceleration structures are not wired up in the thin DirectX 12 backend yet.");
    return nullptr;
}

ThinAccelerationStructurePtr D3D12ThinDevice::VCreateTopLevelAccelerationStructure(const std::vector<ThinAccelerationStructurePtr> &)
{
    FN("D3D12ThinDevice::VCreateTopLevelAccelerationStructure");

    LOG_ERROR("Acceleration structures are not wired up in the thin DirectX 12 backend yet.");
    return nullptr;
}

} // namespace bow
