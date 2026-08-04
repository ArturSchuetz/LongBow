#include <VulkanRenderDevice/Device/BowVulkanRenderSurface.h>

#include <VulkanRenderDevice/BowVulkanInstance.h>
#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanRenderDevice.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanFence.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/BowVulkanSemaphore.h>
#include <VulkanRenderDevice/Device/BowVulkanSwapchain.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanIndexBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanRayTracingShaderProgram.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderProgram.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderResourceBindings.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanFramebuffer.h>
#include <VulkanRenderDevice/Device/Surface/FrameBuffer/BowVulkanRenderPass.h>
#include <VulkanRenderDevice/Device/Surface/VertexAttributeBindings/BowVulkanVertexAttributeBindings.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTextureSampler.h>

#include <RenderDevice/BowClearState.h>
#include <RenderDevice/Device/Context/Mesh/BowMeshBuffers.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>
#include <CoreSystems/Geometry/BowMeshAttribute.h>

#include <optick.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bow
{

VulkanRenderSurface *VulkanRenderSurface::m_currentSurface;

VulkanRenderSurface::VulkanRenderSurface(GLFWwindow *window)
    : m_renderDevice(nullptr), m_swapchain(nullptr), m_viewport(), m_vsync(false), m_boundFramebuffer(nullptr), m_surface(VK_NULL_HANDLE), m_window(window), m_renderPass(nullptr), m_clearColor(), m_clearDepth(1.0f), m_clearStencil(0.0f),
      m_clearColorBuffer(false), m_clearDepthBuffer(false), m_clearStencilBuffer(false), m_currentlyBindedVertexAttributeBindings(nullptr), m_guid(Utils::GenerateGUID())
{
    FN("VulkanRenderSurface::VulkanRenderSurface");
}

VulkanRenderSurface::~VulkanRenderSurface()
{
    FN("VulkanRenderSurface::~VulkanRenderSurface");

    m_window = nullptr;
    VRelease();
}

bool VulkanRenderSurface::InitializeSurface(VulkanRenderDevice *renderDevice)
{
    FN("VulkanRenderSurface::InitializeSurface");
    OPTICK_EVENT();

    m_renderDevice = renderDevice;

    // Set this context as the current one if it is not already
    if (m_currentSurface != this)
    {
        LOG_TRACE("glfwMakeContextCurrent");
        glfwMakeContextCurrent(m_window);
        m_currentSurface = this;
    }

    // Create a Vulkan window surface
    LOG_TRACE("glfwCreateWindowSurface");
    VkResult result = glfwCreateWindowSurface(m_renderDevice->GetInstance()->GetHandle(), m_window, nullptr, &m_surface);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VulkanRenderSurface: %s", VulkanTypeConverter::ToString(result).c_str());
        return false;
    }

    LOG_INFO("VulkanRenderSurface initialized");
    return true;
}

bool VulkanRenderSurface::InitializeSwapchain(uint16_t width, uint16_t height)
{
    FN("VulkanRenderSurface::InitializeSwapchain");
    OPTICK_EVENT();

    if (m_renderDevice == nullptr)
    {
        LOG_ERROR("VulkanRenderSurface: Device is not initialized");
        return false;
    }

    if (m_surface == VK_NULL_HANDLE)
    {
        LOG_ERROR("VulkanRenderSurface: Surface is not initialized");
        return false;
    }

    // Create the swapchain
    m_swapchain = m_renderDevice->GetLogicalDevice()->CreateSwapchain(this, width, height);
    if (m_swapchain == nullptr)
    {
        LOG_ERROR("VulkanRenderSurface: Swapchain could not be created");
        return false;
    }

    m_viewport.x = 0;
    m_viewport.y = 0;
    m_viewport.width = std::max(width, (uint16_t)1);
    m_viewport.height = std::max(height, (uint16_t)1);

    return true;
}

void VulkanRenderSurface::VRelease()
{
    FN("VulkanRenderSurface::VRelease");

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_renderDevice->GetLogicalDevice()->GetHandle());

    if (m_swapchain != nullptr)
    {
        m_swapchain->VRelease();
        m_swapchain.reset();
    }

    if (m_surface != VK_NULL_HANDLE)
    {
        LOG_TRACE("vkDestroySurfaceKHR %s", m_guid.c_str());
        vkDestroySurfaceKHR(m_renderDevice->GetInstance()->GetHandle(), m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}

VertexAttributeBindingsPtr VulkanRenderSurface::VCreateVertexAttributeBindings(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
{
    FN("VulkanRenderSurface::VCreateVertexAttributeBindings");
    OPTICK_EVENT();

    return VCreateVertexAttributeBindings(m_renderDevice->VCreateMeshBuffers(mesh, shaderAttributes, usageHint));
}

VertexAttributeBindingsPtr VulkanRenderSurface::VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers)
{
    FN("VulkanRenderSurface::VCreateVertexAttributeBindings");
    OPTICK_EVENT();

    VertexAttributeBindingsPtr vertexAttributeBindings = VCreateVertexAttributeBindings();
    if (meshBuffers->IndexBuffer != nullptr)
    {
        vertexAttributeBindings->VSetIndexBuffer(meshBuffers->IndexBuffer);
    }

    VertexBufferAttributeMap attributeMap = meshBuffers->GetAttributes();
    for (auto attribute = attributeMap.begin(); attribute != attributeMap.end(); ++attribute)
    {
        vertexAttributeBindings->VSetAttribute(attribute->first, attribute->second);
    }

    return vertexAttributeBindings;
}

VertexAttributeBindingsPtr VulkanRenderSurface::VCreateVertexAttributeBindings()
{
    FN("VulkanRenderSurface::VCreateVertexAttributeBindings");
    OPTICK_EVENT();

    VulkanVertexAttributeBindingsPtr vertexAttributeBindings = VulkanVertexAttributeBindingsPtr(new VulkanVertexAttributeBindings());
    return vertexAttributeBindings;
}

FramebufferPtr VulkanRenderSurface::VCreateFramebuffer()
{
    FN("VulkanRenderSurface::VCreateFramebuffer");
    OPTICK_EVENT();

    VulkanFramebufferPtr framebuffer = VulkanFramebufferPtr(new VulkanFramebuffer(m_renderDevice->GetLogicalDevice().get()));
    return framebuffer;
}

void VulkanRenderSurface::VBeginFrame()
{
    FN("VulkanRenderSurface::VBeginFrame");
    LOG_FATAL("Not implemented yet!");
}

void VulkanRenderSurface::VEndFrame()
{
    FN("VulkanRenderSurface::VEndFrame");
    LOG_FATAL("Not implemented yet!");
}

void VulkanRenderSurface::VClear(ClearState clearState)
{
    FN("VulkanRenderSurface::VClear");
    OPTICK_EVENT();

    if (m_clearColor != clearState.color)
    {
        m_clearColor = clearState.color;
    }

    if (m_clearDepth != clearState.depth)
    {
        m_clearDepth = clearState.depth;
    }

    if (m_clearStencil != clearState.stencil)
    {
        m_clearStencil = clearState.stencil;
    }

    m_clearColorBuffer = clearState.buffers & ColorBuffer;
    m_clearDepthBuffer = clearState.buffers & DepthBuffer;
    m_clearStencilBuffer = clearState.buffers & StencilBuffer;
}

void VulkanRenderSurface::VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("VulkanRenderSurface::VDraw");
    OPTICK_EVENT();

    PrepareCommandBuffer();
    PrepareRenderPass(true);

    ApplyVertexAttributeBindings(vertexAttributeBindings);

    Draw(primitiveType, -1, -1, vertexAttributeBindings, shaderProgram, renderState, std::vector<VkDescriptorSet>());
}

void VulkanRenderSurface::VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState)
{
    FN("VulkanRenderSurface::VDraw");
    OPTICK_EVENT();

    LOG_ASSERT(!(offset < 0 || count < 0), "The offset and count must be greater than or equal to zero.");

    PrepareCommandBuffer();
    PrepareRenderPass(true);

    ApplyVertexAttributeBindings(vertexAttributeBindings);

    Draw(primitiveType, offset, count, vertexAttributeBindings, shaderProgram, renderState, std::vector<VkDescriptorSet>());
}

void VulkanRenderSurface::VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram,
                                RenderState renderState)
{
    FN("VulkanRenderSurface::VDraw");
    OPTICK_EVENT();

    LOG_ASSERT(!(offset < 0 || count < 0), "The offset and count must be greater than or equal to zero.");

    PrepareCommandBuffer();
    PrepareRenderPass(true);

    ApplyVertexAttributeBindings(vertexAttributeBindings);

    VulkanShaderProgramPtr vulkanShaderProgram = std::dynamic_pointer_cast<VulkanShaderProgram>(shaderProgram);

    VulkanShaderResourceBindingsPtr vulkanShaderResourceBindings = std::dynamic_pointer_cast<VulkanShaderResourceBindings>(shaderResourceBindings);
    Draw(primitiveType, offset, count, vertexAttributeBindings, shaderProgram, renderState, vulkanShaderResourceBindings->GetDescriptorSets());
}

void VulkanRenderSurface::VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end)
{
    FN("VulkanRenderSurface::VDrawLine");
    OPTICK_EVENT();

    LOG_FATAL("VulkanRenderSurface::VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end) is not implemented");
}

void VulkanRenderSurface::VSetFramebuffer(FramebufferPtr framebufer)
{
    FN("VulkanRenderSurface::VSetFramebuffer");

    if (m_boundFramebuffer != framebufer)
    {
        if (FinishRenderPass())
        {
            FinishCommandBuffer();
        }
        m_boundFramebuffer = std::dynamic_pointer_cast<VulkanFramebuffer>(framebufer);
    }
}

void VulkanRenderSurface::VSetViewport(Viewport viewport)
{
    FN("VulkanRenderSurface::VSetViewport");

    LOG_ASSERT(!(viewport.width < 0 || viewport.height < 0), "The viewport width and height must be greater than or equal to zero.");

    if (m_viewport != viewport)
    {
        m_viewport = viewport;
    }
}

Viewport VulkanRenderSurface::VGetViewport()
{
    FN("VulkanRenderSurface::VGetViewport");

    return m_viewport;
}

void VulkanRenderSurface::VSwapBuffers(bool vsync)
{
    FN("VulkanRenderSurface::VSwapBuffers");
    OPTICK_EVENT();

    if (m_vsync != vsync)
    {
        m_vsync = vsync;
    }

    FinishRenderPass();
    FinishCommandBuffer();

    if (m_boundFramebuffer == nullptr)
    {
        VulkanCommandBufferPtr commandBuffer = m_swapchain->GetCurrentFramebuffer()->GetGraphicsCommandBuffer();

        VkResult result = m_swapchain->SwapBuffers(vsync);
        if (result != VK_SUCCESS)
        {
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            {
                int fbWidth, fbHeight;
                LOG_TRACE("glfwGetFramebufferSize");
                glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
                RecreateSwapchain(std::max(fbWidth, 1), std::max(fbHeight, 1));
            }
            else
            {
                LOG_ERROR("VulkanRenderSurface: %s", VulkanTypeConverter::ToString(result).c_str());
            }
        }
    }
    LOG_UPDATE();
}

void VulkanRenderSurface::VTraceRays(void *shaderProgram, ShaderResourceBindingsPtr resourceBindings, Texture2DPtr outputImage, uint32_t width, uint32_t height)
{
    FN("VulkanRenderSurface::VTraceRays");
    OPTICK_EVENT();

    PrepareCommandBuffer();

    VulkanRayTracingShaderProgram *rtProgram = static_cast<VulkanRayTracingShaderProgram *>(shaderProgram);
    VulkanShaderResourceBindingsPtr vulkanBindings = std::dynamic_pointer_cast<VulkanShaderResourceBindings>(resourceBindings);
    VulkanTexture2DPtr vulkanOutputImage = std::dynamic_pointer_cast<VulkanTexture2D>(outputImage);

    VulkanFramebufferPtr currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();
    VkCommandBuffer cmd = commandBuffer->GetHandle();

    // Bind RT pipeline and descriptors
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtProgram->GetPipeline());

    std::vector<VkDescriptorSet> descriptorSets = vulkanBindings->GetDescriptorSets();
    if (!descriptorSets.empty())
    {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtProgram->GetPipelineLayout(), 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
    }

    // Trace rays into storage image
    vkCmdTraceRaysKHR(cmd, &rtProgram->GetRaygenSBTRegion(), &rtProgram->GetMissSBTRegion(), &rtProgram->GetHitSBTRegion(), &rtProgram->GetCallableSBTRegion(), width, height, 1);

    // Copy storage image to swapchain image
    VulkanTexture2DPtr swapchainTarget = m_swapchain->GetCurrentRenderTarget();

    // Barrier: storage image GENERAL -> TRANSFER_SRC
    VkImageMemoryBarrier srcBarrier = {};
    srcBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    srcBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    srcBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    srcBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    srcBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    srcBarrier.image = vulkanOutputImage->GetHandle();
    srcBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    srcBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    srcBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcBarrier);

    // Barrier: swapchain image UNDEFINED -> TRANSFER_DST
    VkImageMemoryBarrier dstBarrier = {};
    dstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dstBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    dstBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dstBarrier.image = swapchainTarget->GetHandle();
    dstBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    dstBarrier.srcAccessMask = 0;
    dstBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstBarrier);

    // Blit (handles format conversion if needed)
    VkImageBlit blitRegion = {};
    blitRegion.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    blitRegion.srcOffsets[0] = {0, 0, 0};
    blitRegion.srcOffsets[1] = {(int32_t)width, (int32_t)height, 1};
    blitRegion.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    blitRegion.dstOffsets[0] = {0, 0, 0};
    blitRegion.dstOffsets[1] = {(int32_t)width, (int32_t)height, 1};
    vkCmdBlitImage(cmd, vulkanOutputImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchainTarget->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);

    // Barrier: swapchain image TRANSFER_DST -> PRESENT_SRC
    VkImageMemoryBarrier presentBarrier = {};
    presentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    presentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    presentBarrier.image = swapchainTarget->GetHandle();
    presentBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    presentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    presentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentBarrier);

    // Barrier: storage image back to GENERAL for next frame
    VkImageMemoryBarrier backBarrier = {};
    backBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    backBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    backBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    backBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    backBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    backBarrier.image = vulkanOutputImage->GetHandle();
    backBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    backBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    backBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, nullptr, 0, nullptr, 1, &backBarrier);
}

VkSurfaceKHR VulkanRenderSurface::GetHandle() const
{
    FN("VulkanRenderSurface::GetHandle");

    return m_surface;
}

// =================================================================================================
// Private Methods
// =================================================================================================

void VulkanRenderSurface::Draw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState,
                               const std::vector<VkDescriptorSet> &descriptorSets)
{
    FN("VulkanRenderSurface::Draw");
    OPTICK_EVENT();

    // Command Buffer and Render Pass is prepared
    // ToDo: Now Render the Triangle

    VulkanFramebufferPtr currentFramebuffer = nullptr;
    if (m_boundFramebuffer == nullptr)
    {
        currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    }
    else
    {
        currentFramebuffer = m_boundFramebuffer;
    }

    VulkanRenderPassPtr currentRenderPass = currentFramebuffer->GetRenderPass();

    VulkanShaderProgramPtr vulkanShaderProgram = std::dynamic_pointer_cast<VulkanShaderProgram>(shaderProgram);
    VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();
    LOG_ASSERT(commandBuffer->GetState() == VulkanCommandBufferState::Recording, "The command buffer must be in recording state.");

    vulkanShaderProgram->Bind(commandBuffer, primitiveType, renderState, m_viewport, currentRenderPass, descriptorSets);

    currentRenderPass->NotifyBinding(vulkanShaderProgram);

    VulkanVertexAttributeBindingsPtr vulkanVertexAttributeBindings = std::dynamic_pointer_cast<VulkanVertexAttributeBindings>(vertexAttributeBindings);
    VulkanIndexBufferPtr vulkanIndexbuffer = std::dynamic_pointer_cast<VulkanIndexBuffer>(vulkanVertexAttributeBindings->VGetIndexBuffer());

    if (vulkanIndexbuffer == nullptr)
    {
        if (offset == -1 && count == -1)
        {
            LOG_TRACE("vkCmdDraw %s", m_guid.c_str());
            vkCmdDraw(commandBuffer->GetHandle(), vulkanVertexAttributeBindings->MaximumArrayIndex() + 1, 1, 0, 0);
        }
        else
        {
            LOG_TRACE("vkCmdDraw %s", m_guid.c_str());
            vkCmdDraw(commandBuffer->GetHandle(), count, 1, offset, 0);
        }
    }
    else
    {
        if (offset == -1 && count == -1)
        {
            LOG_TRACE("vkCmdDrawIndexed %s", m_guid.c_str());
            vkCmdDrawIndexed(commandBuffer->GetHandle(), vulkanIndexbuffer->GetIndexCount(), 1, 0, 0, 0);
        }
        else
        {
            LOG_TRACE("vkCmdDrawIndexed %s", m_guid.c_str());
            vkCmdDrawIndexed(commandBuffer->GetHandle(), count, 1, offset, 0, 0);
        }
    }
}

void VulkanRenderSurface::PrepareCommandBuffer()
{
    FN("VulkanRenderSurface::PrepareCommandBuffer");
    OPTICK_EVENT();

    VulkanFramebufferPtr currentFramebuffer = nullptr;
    if (m_boundFramebuffer == nullptr)
    {
        currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    }
    else
    {
        currentFramebuffer = m_boundFramebuffer;
    }

    if (currentFramebuffer != nullptr)
    {
        if (!currentFramebuffer->IsInitialized())
        {
            currentFramebuffer->Initialize(m_clearColorBuffer, m_clearDepthBuffer, m_clearStencilBuffer);
        }

        VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();

        // If state is submitted, reset buffer to ready
        if (commandBuffer->GetState() == VulkanCommandBufferState::Submitted)
        {
            bool success = commandBuffer->Reset();
            LOG_ASSERT(success, "VulkanRenderSurface: Command buffer could not be reset!");
        }

        // If state is ready, buffer has to be started recording
        if (commandBuffer->GetState() == VulkanCommandBufferState::Ready)
        {
            bool success = commandBuffer->Begin(false, false, false);
            LOG_ASSERT(success, "VulkanRenderSurface: Command buffer could not be started recording!");
        }
    }
}

void VulkanRenderSurface::FinishCommandBuffer()
{
    FN("VulkanRenderSurface::FinishCommandBuffer");
    OPTICK_EVENT();

    VulkanFramebufferPtr currentFramebuffer = nullptr;
    if (m_boundFramebuffer == nullptr)
    {
        currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    }
    else
    {
        currentFramebuffer = m_boundFramebuffer;
    }

    if (currentFramebuffer != nullptr)
    {
        if (!currentFramebuffer->IsInitialized())
        {
            currentFramebuffer->Initialize(m_clearColorBuffer, m_clearDepthBuffer, m_clearStencilBuffer);
        }

        VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();

        // If buffer is recording, end recording
        if (commandBuffer->GetState() == VulkanCommandBufferState::Recording)
        {
            bool success = commandBuffer->End();
            LOG_ASSERT(success, "VulkanRenderSurface: Command buffer could not be ended recording!");
        }

        if (commandBuffer->GetState() == VulkanCommandBufferState::RecordingEnded)
        {
            VkCommandBuffer commandBuffers[] = {commandBuffer->GetHandle()};

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = nullptr;
            submitInfo.waitSemaphoreCount = 0;
            submitInfo.pWaitSemaphores = nullptr;
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.pWaitDstStageMask = waitStages;

            // If no framebuffer is bound, signal the render finished semaphore after the command buffer has been executed so that the swapchain can present the image
            if (m_boundFramebuffer == nullptr)
            {
                VulkanSemaphorePtr renderFinishedSemaphore = currentFramebuffer->GetRenderFinishedSemaphore();
                VkSemaphore signalSemaphores[] = {renderFinishedSemaphore->GetHandle()};

                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = signalSemaphores;
            }
            else
            {
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;
            }

            // If no framebuffer is bound, wait for the backbuffer image to be available for rendering before submitting the command buffer
            if (m_boundFramebuffer == nullptr)
            {
                VulkanSemaphorePtr imageAvailableSemaphore = m_swapchain->GetCurrentImageAvailableSemaphore();
                VkSemaphore waitSemaphores[] = {imageAvailableSemaphore->GetHandle()};

                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = waitSemaphores;
            }
            else
            {
                submitInfo.waitSemaphoreCount = 0;
                submitInfo.pWaitSemaphores = nullptr;
            }

            VkQueue graphicsQueue = m_renderDevice->GetLogicalDevice()->GetGraphicsQueue();
            bool success = commandBuffer->Submit(graphicsQueue, 0, submitInfo);
            LOG_ASSERT(success, "VulkanRenderSurface: Command buffer could not be submitted!");

            commandBuffer->WaitForQueueIdle(graphicsQueue);
        }
    }
}

void VulkanRenderSurface::PrepareRenderPass(bool clearPass)
{
    FN("VulkanRenderSurface::PrepareRenderPass");
    OPTICK_EVENT();

    VulkanFramebufferPtr currentFramebuffer = nullptr;
    if (m_boundFramebuffer == nullptr)
    {
        currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    }
    else
    {
        currentFramebuffer = m_boundFramebuffer;
    }

    if (clearPass)
    {
        if (!currentFramebuffer->IsInitialized())
        {
            currentFramebuffer->Initialize(m_clearColorBuffer, m_clearDepthBuffer, m_clearStencilBuffer);
        }

        if (currentFramebuffer->IsInitialized() && !currentFramebuffer->IsDirty())
        {
            currentFramebuffer->UpdateColorAttachmentDescriptions(m_clearColorBuffer, m_clearDepthBuffer, m_clearStencilBuffer);
        }
        else if (currentFramebuffer->IsDirty())
        {
            {
                OPTICK_EVENT("bow::VulkanFramebuffer::VRelease");
                currentFramebuffer->VRelease();
            }
            currentFramebuffer->Initialize();
        }
    }
    else
    {
        if (!currentFramebuffer->IsInitialized())
        {
            currentFramebuffer->Initialize(false, false, false);
        }

        if (currentFramebuffer->IsInitialized() && !currentFramebuffer->IsDirty())
        {
            currentFramebuffer->UpdateColorAttachmentDescriptions(false, false, false);
        }
        else if (currentFramebuffer->IsDirty())
        {
            {
                OPTICK_EVENT("bow::VulkanFramebuffer::VRelease");
                currentFramebuffer->VRelease();
            }
            currentFramebuffer->Initialize();
        }
    }

    VulkanRenderPassPtr currentRenderPass = currentFramebuffer->GetRenderPass();
    if (currentRenderPass->GetState() == VulkanRenderPassState::Ready)
    {
        std::vector<VkClearValue> clearColors = {};
        if (m_clearColorBuffer)
        {
            for (size_t i = 0; i < currentFramebuffer->GetColorAttachmentsCount(); i++)
            {
                VkClearValue clearColorColor = {};
                clearColorColor.color = {m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w};
                clearColors.push_back(clearColorColor);
            }
        }

        VkClearValue clearDepth = {};
        if (currentFramebuffer->VGetDepthAttachment() != nullptr)
        {
            clearDepth.depthStencil.depth = m_clearDepth;
        }
        if (currentFramebuffer->VGetDepthStencilAttachment() != nullptr)
        {
            clearDepth.depthStencil.depth = m_clearDepth;
            clearDepth.depthStencil.stencil = m_clearStencil;
        }
        clearColors.push_back(clearDepth);

        // Get the render area from the render target

        uint32_t width = currentFramebuffer->GetWidth();
        uint32_t height = currentFramebuffer->GetHeight();

        VkRect2D renderArea = {};
        renderArea.offset = {0, 0};
        renderArea.extent = {width, height};

        VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();
        LOG_ASSERT(commandBuffer->GetState() == VulkanCommandBufferState::Recording, "The command buffer must be in recording state.");

        currentRenderPass->Begin(commandBuffer->GetHandle(), currentFramebuffer->GetHandle(), renderArea, clearColors);
    }
}

bool VulkanRenderSurface::FinishRenderPass()
{
    FN("VulkanRenderSurface::FinishRenderPass");
    OPTICK_EVENT();

    VulkanFramebufferPtr currentFramebuffer = nullptr;
    if (m_boundFramebuffer == nullptr)
    {
        currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    }
    else
    {
        currentFramebuffer = m_boundFramebuffer;
    }

    VulkanRenderPassPtr currentRenderPass = currentFramebuffer->GetRenderPass();
    if (currentRenderPass != nullptr)
    {
        if (currentRenderPass->GetState() == VulkanRenderPassState::Recording)
        {
            VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();
            currentRenderPass->End(commandBuffer->GetHandle());
            return true;
        }
    }
    return false;
}

// =================================================================================================
// PRIVATE METHODS
// =================================================================================================

bool VulkanRenderSurface::RecreateSwapchain(uint16_t width, uint16_t height)
{
    FN("VulkanRenderSurface::RecreateSwapchain");
    OPTICK_EVENT();

    LOG_TRACE("vkDeviceWaitIdle %s", m_guid.c_str());
    vkDeviceWaitIdle(m_renderDevice->GetLogicalDevice()->GetHandle());

    if (m_renderDevice == nullptr)
    {
        LOG_ERROR("VulkanRenderSurface: Device is not initialized");
        return false;
    }

    if (m_surface == VK_NULL_HANDLE)
    {
        LOG_ERROR("VulkanRenderSurface: Surface is not initialized");
        return false;
    }

    if (m_swapchain != nullptr)
    {
        {
            OPTICK_EVENT("bow::VulkanSwapchain::VRelease");
            m_swapchain->VRelease();
        }
        m_swapchain.reset();
    }

    // Ensure non-zero dimensions for the swapchain.
    width = std::max((int)width, 1);
    height = std::max((int)height, 1);

    m_swapchain = m_renderDevice->GetLogicalDevice()->CreateSwapchain(this, width, height);
    if (m_swapchain == nullptr)
    {
        LOG_ERROR("VulkanRenderSurface: Swapchain could not be created");
        return false;
    }

    return true;
}

void VulkanRenderSurface::ApplyVertexAttributeBindings(VertexAttributeBindingsPtr vertexAttributeBindings)
{
    FN("VulkanRenderSurface::ApplyVertexAttributeBindings");
    OPTICK_EVENT();

    VulkanFramebufferPtr currentFramebuffer = nullptr;
    if (m_boundFramebuffer == nullptr)
    {
        currentFramebuffer = m_swapchain->GetCurrentFramebuffer();
    }
    else
    {
        currentFramebuffer = m_boundFramebuffer;
    }

    VulkanCommandBufferPtr commandBuffer = currentFramebuffer->GetGraphicsCommandBuffer();
    LOG_ASSERT(commandBuffer->GetState() == VulkanCommandBufferState::Recording, "The command buffer must be in recording state.");

    VulkanVertexAttributeBindingsPtr vulkanVertexAttributeBindings = std::dynamic_pointer_cast<VulkanVertexAttributeBindings>(vertexAttributeBindings);
    vulkanVertexAttributeBindings->Bind(commandBuffer->GetHandle());

    m_currentlyBindedVertexAttributeBindings = vertexAttributeBindings;
}

} // namespace bow
