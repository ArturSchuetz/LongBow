#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <VulkanThinDevice/BowVulkanThinObjects.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

ThinSwapchainPtr VulkanThinDevice::VCreateSwapchain(void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount)
{
    FN("VulkanThinDevice::VCreateSwapchain");

    std::shared_ptr<VulkanThinSwapchain> swapchain = std::make_shared<VulkanThinSwapchain>(this, nativeWindowHandle, width, height, imageCount);
    return swapchain->IsReady() ? swapchain : nullptr;
}

ThinCommandPoolPtr VulkanThinDevice::VCreateCommandPool(ThinQueueType type)
{
    FN("VulkanThinDevice::VCreateCommandPool");

    return std::make_shared<VulkanThinCommandPool>(this, GetQueueFamily(type));
}

ThinFencePtr VulkanThinDevice::VCreateFence(uint64_t initialValue)
{
    FN("VulkanThinDevice::VCreateFence");

    return std::make_shared<VulkanThinFence>(this, initialValue);
}

ThinBufferPtr VulkanThinDevice::VCreateBuffer(const ThinBufferDescription &description)
{
    FN("VulkanThinDevice::VCreateBuffer");

    std::shared_ptr<VulkanThinBuffer> buffer = std::make_shared<VulkanThinBuffer>(this, description);
    return buffer->IsReady() ? buffer : nullptr;
}

ThinTexturePtr VulkanThinDevice::VCreateTexture(const ThinTextureDescription &description)
{
    FN("VulkanThinDevice::VCreateTexture");

    std::shared_ptr<VulkanThinTexture> texture = std::make_shared<VulkanThinTexture>(this, description);
    return texture->IsReady() ? texture : nullptr;
}

ThinSamplerPtr VulkanThinDevice::VCreateSampler(const ThinSamplerDescription &description)
{
    FN("VulkanThinDevice::VCreateSampler");

    return std::make_shared<VulkanThinSampler>(this, description);
}

ThinShaderModulePtr VulkanThinDevice::VCreateShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char *entryPoint)
{
    FN("VulkanThinDevice::VCreateShaderModule");

    if (byteCode == nullptr || sizeInBytes == 0)
    {
        LOG_ERROR("No bytecode given for the shader module.");
        return nullptr;
    }

    // SPIR-V is a stream of 32-bit words, so a size that is not a multiple of
    // four is not SPIR-V -- most likely HLSL or GLSL source by mistake.
    if ((sizeInBytes % 4) != 0)
    {
        LOG_ERROR("Shader bytecode is %u bytes, which is not a whole number of SPIR-V words. Was source passed instead of compiled SPIR-V?", (unsigned)sizeInBytes);
        return nullptr;
    }

    return std::make_shared<VulkanThinShaderModule>(this, byteCode, sizeInBytes, stage, entryPoint);
}

ThinPipelineLayoutPtr VulkanThinDevice::VCreatePipelineLayout(const ThinPipelineLayoutDescription &description)
{
    FN("VulkanThinDevice::VCreatePipelineLayout");

    return std::make_shared<VulkanThinPipelineLayout>(this, description);
}

ThinPipelinePtr VulkanThinDevice::VCreateGraphicsPipeline(const ThinGraphicsPipelineDescription &description)
{
    FN("VulkanThinDevice::VCreateGraphicsPipeline");

    if (description.layout == nullptr || description.vertexShader == nullptr || description.fragmentShader == nullptr)
    {
        LOG_ERROR("A graphics pipeline needs a layout, a vertex shader and a fragment shader.");
        return nullptr;
    }

    std::vector<VkPipelineShaderStageCreateInfo> stages;
    auto addStage = [&stages](const ThinShaderModulePtr &module) {
        if (module == nullptr)
        {
            return;
        }
        VulkanThinShaderModule *vulkanModule = static_cast<VulkanThinShaderModule *>(module.get());

        VkPipelineShaderStageCreateInfo stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        stage.stage = VulkanThinTypes::ToShaderStage(module->VGetStage());
        stage.module = vulkanModule->GetHandle();
        stage.pName = vulkanModule->GetEntryPoint().c_str();
        stages.push_back(stage);
    };

    addStage(description.vertexShader);
    addStage(description.geometryShader);
    addStage(description.fragmentShader);

    std::vector<VkVertexInputBindingDescription> bindings;
    bindings.reserve(description.vertexBuffers.size());
    for (const ThinVertexBufferBinding &binding : description.vertexBuffers)
    {
        VkVertexInputBindingDescription vkBinding = {};
        vkBinding.binding = binding.binding;
        vkBinding.stride = binding.strideInBytes;
        vkBinding.inputRate = binding.perInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        bindings.push_back(vkBinding);
    }

    std::vector<VkVertexInputAttributeDescription> attributes;
    attributes.reserve(description.vertexAttributes.size());
    for (const ThinVertexAttribute &attribute : description.vertexAttributes)
    {
        VkVertexInputAttributeDescription vkAttribute = {};
        vkAttribute.location = attribute.location;
        vkAttribute.binding = attribute.bufferBinding;
        vkAttribute.format = VulkanThinTypes::ToVkFormat(attribute.format);
        vkAttribute.offset = attribute.offsetInBytes;
        attributes.push_back(vkAttribute);
    }

    VkPipelineVertexInputStateCreateInfo vertexInput = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInput.vertexBindingDescriptionCount = (uint32_t)bindings.size();
    vertexInput.pVertexBindingDescriptions = bindings.data();
    vertexInput.vertexAttributeDescriptionCount = (uint32_t)attributes.size();
    vertexInput.pVertexAttributeDescriptions = attributes.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VulkanThinTypes::ToPrimitiveTopology(description.topology);

    VkPipelineViewportStateCreateInfo viewport = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization.polygonMode = description.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VulkanThinTypes::ToCullMode(description.cullMode);
    rasterization.frontFace = description.frontFaceCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencil.depthTestEnable = description.depthTestEnabled ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = description.depthWriteEnabled ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = VulkanThinTypes::ToCompareOp(description.depthCompareOp);

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
    for (size_t i = 0; i < description.colorAttachmentFormats.size(); ++i)
    {
        VkPipelineColorBlendAttachmentState attachment = {};
        attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        if (i < description.colorAttachmentBlends.size() && description.colorAttachmentBlends[i].enabled)
        {
            const ThinColorAttachmentBlend &blend = description.colorAttachmentBlends[i];
            attachment.blendEnable = VK_TRUE;
            attachment.srcColorBlendFactor = VulkanThinTypes::ToBlendFactor(blend.sourceColor);
            attachment.dstColorBlendFactor = VulkanThinTypes::ToBlendFactor(blend.destinationColor);
            attachment.colorBlendOp = VK_BLEND_OP_ADD;
            attachment.srcAlphaBlendFactor = VulkanThinTypes::ToBlendFactor(blend.sourceAlpha);
            attachment.dstAlphaBlendFactor = VulkanThinTypes::ToBlendFactor(blend.destinationAlpha);
            attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }

        blendAttachments.push_back(attachment);
    }

    VkPipelineColorBlendStateCreateInfo colorBlend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlend.attachmentCount = (uint32_t)blendAttachments.size();
    colorBlend.pAttachments = blendAttachments.data();

    // Viewport and scissor are set on the command list, so a pipeline does not
    // have to be rebuilt when the window changes size.
    const VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic.dynamicStateCount = 2;
    dynamic.pDynamicStates = dynamicStates;

    std::vector<VkFormat> colorFormats;
    colorFormats.reserve(description.colorAttachmentFormats.size());
    for (ThinFormat format : description.colorAttachmentFormats)
    {
        colorFormats.push_back(VulkanThinTypes::ToVkFormat(format));
    }

    // Dynamic rendering: the pipeline states the formats it writes to instead
    // of being tied to a render pass object.
    VkPipelineRenderingCreateInfo rendering = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    rendering.colorAttachmentCount = (uint32_t)colorFormats.size();
    rendering.pColorAttachmentFormats = colorFormats.data();
    rendering.depthAttachmentFormat = VulkanThinTypes::ToVkFormat(description.depthAttachmentFormat);

    VkGraphicsPipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    createInfo.pNext = &rendering;
    createInfo.stageCount = (uint32_t)stages.size();
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInput;
    createInfo.pInputAssemblyState = &inputAssembly;
    createInfo.pViewportState = &viewport;
    createInfo.pRasterizationState = &rasterization;
    createInfo.pMultisampleState = &multisample;
    createInfo.pDepthStencilState = &depthStencil;
    createInfo.pColorBlendState = &colorBlend;
    createInfo.pDynamicState = &dynamic;
    createInfo.layout = static_cast<VulkanThinPipelineLayout *>(description.layout.get())->GetHandle();

    VkPipeline pipeline = VK_NULL_HANDLE;
    if (!VulkanCheck(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline), "vkCreateGraphicsPipelines"))
    {
        return nullptr;
    }

    return std::make_shared<VulkanThinPipeline>(this, pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, description.layout);
}

ThinPipelinePtr VulkanThinDevice::VCreateComputePipeline(const ThinComputePipelineDescription &description)
{
    FN("VulkanThinDevice::VCreateComputePipeline");

    if (description.layout == nullptr || description.computeShader == nullptr)
    {
        LOG_ERROR("A compute pipeline needs a layout and a compute shader.");
        return nullptr;
    }

    VulkanThinShaderModule *module = static_cast<VulkanThinShaderModule *>(description.computeShader.get());

    VkPipelineShaderStageCreateInfo stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = module->GetHandle();
    stage.pName = module->GetEntryPoint().c_str();

    VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    createInfo.stage = stage;
    createInfo.layout = static_cast<VulkanThinPipelineLayout *>(description.layout.get())->GetHandle();

    VkPipeline pipeline = VK_NULL_HANDLE;
    if (!VulkanCheck(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline), "vkCreateComputePipelines"))
    {
        return nullptr;
    }

    return std::make_shared<VulkanThinPipeline>(this, pipeline, VK_PIPELINE_BIND_POINT_COMPUTE, description.layout);
}

ThinPipelinePtr VulkanThinDevice::VCreateRayTracingPipeline(const ThinRayTracingPipelineDescription &)
{
    FN("VulkanThinDevice::VCreateRayTracingPipeline");

    LOG_ERROR("Ray tracing pipelines are not wired up in the thin Vulkan backend yet.");
    return nullptr;
}

ThinDescriptorPoolPtr VulkanThinDevice::VCreateDescriptorPool(uint32_t maxSets)
{
    FN("VulkanThinDevice::VCreateDescriptorPool");

    return std::make_shared<VulkanThinDescriptorPool>(this, maxSets);
}

ThinAccelerationStructurePtr VulkanThinDevice::VCreateBottomLevelAccelerationStructure(const ThinBufferPtr &, uint32_t, uint32_t, const ThinBufferPtr &, uint32_t)
{
    FN("VulkanThinDevice::VCreateBottomLevelAccelerationStructure");

    LOG_ERROR("Acceleration structures are not wired up in the thin Vulkan backend yet.");
    return nullptr;
}

ThinAccelerationStructurePtr VulkanThinDevice::VCreateTopLevelAccelerationStructure(const std::vector<ThinAccelerationStructurePtr> &)
{
    FN("VulkanThinDevice::VCreateTopLevelAccelerationStructure");

    LOG_ERROR("Acceleration structures are not wired up in the thin Vulkan backend yet.");
    return nullptr;
}

} // namespace bow
