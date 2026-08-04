#include <VulkanRenderDevice/Device/BowVulkanPipeline.h>

#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderProgram.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanRenderPass.h>

#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/IBowRenderContext.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/BowUtils.h>

namespace bow
{

VulkanPipeline::VulkanPipeline() : m_logicalDevice(nullptr), m_pipeline(VK_NULL_HANDLE), m_primitiveType(PrimitiveType::Triangles), m_renderState(), m_viewport(), m_guid(Utils::GenerateGUID()) { FN("VulkanPipeline::VulkanPipeline"); }

VulkanPipeline::~VulkanPipeline()
{
    FN("VulkanPipeline::~VulkanPipeline");

    Release();
}

bool VulkanPipeline::Initialize(VulkanLogicalDevice *logicalDevice, const PrimitiveType &primitiveType, const RenderState &renderState, const Viewport &viewportInput, VulkanRenderPassPtr renderPass,
                                const VkPipelineVertexInputStateCreateInfo &pipelineVertexInputStateCreateInfo, const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages, VkPipelineLayout pipelineLayout)
{
    FN("VulkanPipeline::Initialize");

    m_logicalDevice = logicalDevice;

    // Input Assembly State
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext = nullptr;
    inputAssemblyStateCreateInfo.flags = 0;
    inputAssemblyStateCreateInfo.topology = VulkanTypeConverter::ToVkPrimitiveTopology(primitiveType);                           // Primitive topology: points, lines, triangles, etc.
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VulkanTypeConverter::ToVkBool32(renderState.primitiveRestart.Enabled); // If true, a special vertex index value is treated as a primitive restart

    /*
    // Tessellation State (if used)
    VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo = {};
    tessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationStateCreateInfo.pNext = nullptr;
    tessellationStateCreateInfo.patchControlPoints = 3; // or the appropriate number of control points
    */

    // Viewport State
    VkViewport viewport = {};
    viewport.x = static_cast<float>(viewportInput.x);
    viewport.y = static_cast<float>(viewportInput.y);
    viewport.width = static_cast<float>(viewportInput.width);
    viewport.height = static_cast<float>(viewportInput.height);
    viewport.minDepth = renderState.depthRange.Near;
    viewport.maxDepth = renderState.depthRange.Far;

    VkRect2D scissor = {};
    if (renderState.scissorTest.Enabled)
    {
        scissor.offset = {static_cast<int32_t>(renderState.scissorTest.rectangle_left), static_cast<int32_t>(renderState.scissorTest.rectangle_top)};
        scissor.extent = {static_cast<uint32_t>(renderState.scissorTest.rectangle_right - renderState.scissorTest.rectangle_left), static_cast<uint32_t>(renderState.scissorTest.rectangle_bottom - renderState.scissorTest.rectangle_top)};
    }
    else
    {
        scissor.offset = {0, 0};
        scissor.extent = {static_cast<uint32_t>(viewportInput.width), static_cast<uint32_t>(viewportInput.height)};
    }

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    // Rasterization State
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;        // If true, fragments beyond near and far planes are clamped to them
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; // If true, geometry never passes through the rasterizer stage
    rasterizationStateCreateInfo.polygonMode = VulkanTypeConverter::ToVkPolygonMode(renderState.rasterizationMode);
    rasterizationStateCreateInfo.cullMode = VulkanTypeConverter::ToVkCullMode(renderState.faceCulling.Face);
    rasterizationStateCreateInfo.frontFace = VulkanTypeConverter::ToVkFrontFace(renderState.faceCulling.FrontFaceWindingOrder);
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;        // If true, a constant depth value is added to a fragment's depth
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;    // Constant depth bias factor
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;             // Maximum depth bias factor
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;       // Slope depth bias factor
    rasterizationStateCreateInfo.lineWidth = renderState.lineWidth; // Line width

    // Multisample State
    // I do not want to support multisampling for now so I will just disable it
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext = nullptr;
    multisampleStateCreateInfo.flags = 0;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Number of samples used in rasterization
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;               // If true, enable sample shading
    multisampleStateCreateInfo.minSampleShading = 1.0f;                      // Minimum fraction of sample shading
    multisampleStateCreateInfo.pSampleMask = nullptr;                        // Array of sample mask
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;             // If true, enable alpha to coverage
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;                  // If true, clamp alpha to one

    // Depth Stencil State
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext = nullptr;
    depthStencilStateCreateInfo.flags = 0;
    depthStencilStateCreateInfo.depthTestEnable = VulkanTypeConverter::ToVkBool32(renderState.depthTest.Enabled);     // If true, depth testing is enabled
    depthStencilStateCreateInfo.depthWriteEnable = VulkanTypeConverter::ToVkBool32(renderState.depthWrite.Enabled);   // If true, depth values are written to the depth buffer
    depthStencilStateCreateInfo.depthCompareOp = VulkanTypeConverter::ToVkCompareOp(renderState.depthTest.Function);  // Comparison function used for depth testing
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;                                                     // If true, depth bounds testing is enabled
    depthStencilStateCreateInfo.stencilTestEnable = VulkanTypeConverter::ToVkBool32(renderState.stencilTest.Enabled); // If true, stencil testing is enabled

    depthStencilStateCreateInfo.front.failOp = VulkanTypeConverter::ToVkStencilOp(renderState.stencilTest.FrontFace.StencilFailOperation);               // Stencil operation to use if the front face fails the stencil test
    depthStencilStateCreateInfo.front.passOp = VulkanTypeConverter::ToVkStencilOp(renderState.stencilTest.FrontFace.DepthPassStencilPassOperation);      // Stencil operation to use if the front face passes the stencil test
    depthStencilStateCreateInfo.front.depthFailOp = VulkanTypeConverter::ToVkStencilOp(renderState.stencilTest.FrontFace.DepthFailStencilPassOperation); // Stencil operation to use if the front face fails the depth test
    depthStencilStateCreateInfo.front.compareOp = VulkanTypeConverter::ToVkCompareOp(renderState.stencilTest.FrontFace.Function);                        // Comparison function used for the stencil test
    depthStencilStateCreateInfo.front.compareMask = renderState.stencilTest.FrontFace.CompareMask;                                                       // Bitwise mask to apply to the comparison
    depthStencilStateCreateInfo.front.writeMask = renderState.stencilTest.FrontFace.WriteMask;                                                           // Bitwise mask to apply to the reference value
    depthStencilStateCreateInfo.front.reference = renderState.stencilTest.FrontFace.ReferenceValue;                                                      // Reference value to use in the stencil test

    depthStencilStateCreateInfo.back.failOp = VulkanTypeConverter::ToVkStencilOp(renderState.stencilTest.BackFace.StencilFailOperation);               // Stencil operation to use if the back face fails the stencil test
    depthStencilStateCreateInfo.back.passOp = VulkanTypeConverter::ToVkStencilOp(renderState.stencilTest.BackFace.DepthPassStencilPassOperation);      // Stencil operation to use if the back face passes the stencil test
    depthStencilStateCreateInfo.back.depthFailOp = VulkanTypeConverter::ToVkStencilOp(renderState.stencilTest.BackFace.DepthFailStencilPassOperation); // Stencil operation to use if the back face fails the depth test
    depthStencilStateCreateInfo.back.compareOp = VulkanTypeConverter::ToVkCompareOp(renderState.stencilTest.BackFace.Function);                        // Comparison function used for the stencil test
    depthStencilStateCreateInfo.back.compareMask = renderState.stencilTest.BackFace.CompareMask;                                                       // Bitwise mask to apply to the comparison
    depthStencilStateCreateInfo.back.writeMask = renderState.stencilTest.BackFace.WriteMask;                                                           // Bitwise mask to apply to the reference value
    depthStencilStateCreateInfo.back.reference = renderState.stencilTest.BackFace.ReferenceValue;                                                      // Reference value to use in the stencil test

    depthStencilStateCreateInfo.minDepthBounds = renderState.depthRange.Near; // Lower bound of the range of depth values
    depthStencilStateCreateInfo.maxDepthBounds = renderState.depthRange.Far;  // Upper bound of the range of depth values

    // Color Blend State
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.blendEnable = VulkanTypeConverter::ToVkBool32(renderState.blending.Enabled);
    colorBlendAttachmentState.srcColorBlendFactor = VulkanTypeConverter::ToVkBlendFactor(renderState.blending.SourceRGBFactor);
    colorBlendAttachmentState.dstColorBlendFactor = VulkanTypeConverter::ToVkBlendFactor(renderState.blending.DestinationRGBFactor);
    colorBlendAttachmentState.colorBlendOp = VulkanTypeConverter::ToVkBlendOp(renderState.blending.RGBEquation);
    colorBlendAttachmentState.srcAlphaBlendFactor = VulkanTypeConverter::ToVkBlendFactor(renderState.blending.SourceAlphaFactor);
    colorBlendAttachmentState.dstAlphaBlendFactor = VulkanTypeConverter::ToVkBlendFactor(renderState.blending.DestinationAlphaFactor);
    colorBlendAttachmentState.alphaBlendOp = VulkanTypeConverter::ToVkBlendOp(renderState.blending.AlphaEquation);
    colorBlendAttachmentState.colorWriteMask = (renderState.colorMask.red ? VK_COLOR_COMPONENT_R_BIT : 0) | (renderState.colorMask.green ? VK_COLOR_COMPONENT_G_BIT : 0) | (renderState.colorMask.blue ? VK_COLOR_COMPONENT_B_BIT : 0) |
                                               (renderState.colorMask.alpha ? VK_COLOR_COMPONENT_A_BIT : 0);

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.pNext = nullptr;
    colorBlendStateCreateInfo.flags = 0;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Logical operation to apply to color values
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;   // If true, apply the logical operation
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = renderState.blending.color[0];
    colorBlendStateCreateInfo.blendConstants[1] = renderState.blending.color[1];
    colorBlendStateCreateInfo.blendConstants[2] = renderState.blending.color[2];
    colorBlendStateCreateInfo.blendConstants[3] = renderState.blending.color[3];

    // Dynamic State
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext = nullptr;
    dynamicStateCreateInfo.flags = 0;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pNext = nullptr;
    graphicsPipelineCreateInfo.flags = 0;
    graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    graphicsPipelineCreateInfo.pStages = shaderStages.data();
    graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = renderPass->GetHandle();
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    LOG_TRACE("vkCreateGraphicsPipelines %s", m_guid.c_str());
    VkResult result = vkCreateGraphicsPipelines(m_logicalDevice->GetHandle(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline);
    if (result != VK_SUCCESS)
    {
        SetDynamicStages(nullptr, renderState, viewportInput, renderPass->GetColorAttachmentWidth(), renderPass->GetColorAttachmentHeight());
        m_primitiveType = primitiveType;
        m_renderState = renderState;
        m_viewport = viewportInput;

        LOG_ERROR("VulkanShaderProgram: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    return true;
}

void VulkanPipeline::Release()
{
    FN("VulkanPipeline::Release");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_logicalDevice->GetHandle());
    if (m_pipeline != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroyPipeline %s", m_guid.c_str());
        vkDestroyPipeline(m_logicalDevice->GetHandle(), m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }
}

void VulkanPipeline::SetDynamicStages(VkCommandBuffer commandBuffer, const RenderState &renderState, const Viewport &viewport, uint32_t bufferWidth, uint32_t bufferHeight)
{
    FN("VulkanPipeline::SetDynamicStages");

    m_renderState.scissorTest.Enabled = renderState.scissorTest.Enabled;
    m_renderState.scissorTest.rectangle_left = renderState.scissorTest.rectangle_left;
    m_renderState.scissorTest.rectangle_top = renderState.scissorTest.rectangle_top;
    m_renderState.scissorTest.rectangle_right = renderState.scissorTest.rectangle_right;
    m_renderState.scissorTest.rectangle_bottom = renderState.scissorTest.rectangle_bottom;

    VkRect2D scissor = {};
    if (m_renderState.scissorTest.Enabled)
    {
        scissor.offset = {static_cast<int32_t>(m_renderState.scissorTest.rectangle_left), static_cast<int32_t>(m_renderState.scissorTest.rectangle_top)};
        scissor.extent = {static_cast<uint32_t>(m_renderState.scissorTest.rectangle_right - m_renderState.scissorTest.rectangle_left), static_cast<uint32_t>(m_renderState.scissorTest.rectangle_bottom - m_renderState.scissorTest.rectangle_top)};
    }
    else
    {
        scissor.offset = {0, 0};
        scissor.extent = {static_cast<uint32_t>(m_viewport.width), static_cast<uint32_t>(m_viewport.height)};
    }

    LOG_TRACE("vkCmdSetScissor %s", m_guid.c_str());
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    m_viewport = viewport;
    m_renderState.depthRange.Near = renderState.depthRange.Near;
    m_renderState.depthRange.Far = renderState.depthRange.Far;

    VkViewport vulkanViewport = {};
    vulkanViewport.x = static_cast<float>(m_viewport.x);
    vulkanViewport.y = static_cast<float>(m_viewport.height);
    vulkanViewport.width = static_cast<float>(m_viewport.width);
    vulkanViewport.height = static_cast<float>(-m_viewport.height);
    vulkanViewport.minDepth = m_renderState.depthRange.Near;
    vulkanViewport.maxDepth = m_renderState.depthRange.Far;

    LOG_TRACE("vkCmdSetViewport %s", m_guid.c_str());
    vkCmdSetViewport(commandBuffer, 0, 1, &vulkanViewport);
}

} // namespace bow