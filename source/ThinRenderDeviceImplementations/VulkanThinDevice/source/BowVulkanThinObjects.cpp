#include <VulkanThinDevice/BowVulkanThinObjects.h>

#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <CoreSystems/BowLogger.h>

#include <cstring>

namespace bow
{

// ============================================================ queue and fence

VulkanThinQueue::VulkanThinQueue(VulkanThinDevice *device, VkQueue queue, ThinQueueType type) : m_device(device), m_queue(queue), m_type(type) {}

VulkanThinQueue::~VulkanThinQueue() {}

void VulkanThinQueue::VSubmit(const std::vector<ThinCommandListPtr> &commandLists, const ThinFencePtr &waitFence, uint64_t waitValue, const ThinFencePtr &signalFence, uint64_t signalValue)
{
    FN("VulkanThinQueue::VSubmit");

    std::vector<VkCommandBufferSubmitInfo> buffers;
    buffers.reserve(commandLists.size());
    for (const ThinCommandListPtr &list : commandLists)
    {
        VulkanThinCommandList *vulkanList = static_cast<VulkanThinCommandList *>(list.get());
        if (vulkanList == nullptr)
        {
            continue;
        }

        VkCommandBufferSubmitInfo info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
        info.commandBuffer = vulkanList->GetHandle();
        buffers.push_back(info);
    }

    VkSemaphoreSubmitInfo wait = {VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
    VkSemaphoreSubmitInfo signal = {VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};

    VkSubmitInfo2 submit = {VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
    submit.commandBufferInfoCount = (uint32_t)buffers.size();
    submit.pCommandBufferInfos = buffers.data();

    if (waitFence != nullptr)
    {
        wait.semaphore = static_cast<VulkanThinFence *>(waitFence.get())->GetHandle();
        wait.value = waitValue;
        wait.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        submit.waitSemaphoreInfoCount = 1;
        submit.pWaitSemaphoreInfos = &wait;
    }

    if (signalFence != nullptr)
    {
        signal.semaphore = static_cast<VulkanThinFence *>(signalFence.get())->GetHandle();
        signal.value = signalValue;
        signal.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        submit.signalSemaphoreInfoCount = 1;
        submit.pSignalSemaphoreInfos = &signal;
    }

    VulkanCheck(vkQueueSubmit2(m_queue, 1, &submit, VK_NULL_HANDLE), "vkQueueSubmit2");
}

void VulkanThinQueue::VWaitIdle()
{
    FN("VulkanThinQueue::VWaitIdle");

    vkQueueWaitIdle(m_queue);
}

VulkanThinFence::VulkanThinFence(VulkanThinDevice *device, uint64_t initialValue) : m_device(device), m_semaphore(VK_NULL_HANDLE)
{
    FN("VulkanThinFence::VulkanThinFence");

    VkSemaphoreTypeCreateInfo typeInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};
    typeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    typeInfo.initialValue = initialValue;

    VkSemaphoreCreateInfo createInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    createInfo.pNext = &typeInfo;

    VulkanCheck(vkCreateSemaphore(device->GetHandle(), &createInfo, nullptr, &m_semaphore), "vkCreateSemaphore (timeline)");
}

VulkanThinFence::~VulkanThinFence()
{
    if (m_semaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_device->GetHandle(), m_semaphore, nullptr);
    }
}

uint64_t VulkanThinFence::VGetCompletedValue() const
{
    uint64_t value = 0;
    vkGetSemaphoreCounterValue(m_device->GetHandle(), m_semaphore, &value);
    return value;
}

void VulkanThinFence::VWait(uint64_t value)
{
    FN("VulkanThinFence::VWait");

    VkSemaphoreWaitInfo waitInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &m_semaphore;
    waitInfo.pValues = &value;

    vkWaitSemaphores(m_device->GetHandle(), &waitInfo, UINT64_MAX);
}

void VulkanThinFence::VSignal(uint64_t value)
{
    FN("VulkanThinFence::VSignal");

    VkSemaphoreSignalInfo signalInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO};
    signalInfo.semaphore = m_semaphore;
    signalInfo.value = value;

    vkSignalSemaphore(m_device->GetHandle(), &signalInfo);
}

// ================================================================ command list

VulkanThinCommandList::VulkanThinCommandList(VulkanThinDevice *device, VkCommandBuffer commandBuffer) : m_device(device), m_commandBuffer(commandBuffer), m_boundLayout(VK_NULL_HANDLE), m_boundBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS) {}

VulkanThinCommandList::~VulkanThinCommandList() {}

void VulkanThinCommandList::VBegin()
{
    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
}

void VulkanThinCommandList::VEnd() { vkEndCommandBuffer(m_commandBuffer); }

void VulkanThinCommandList::VBeginRendering(const ThinRenderingInfo &renderingInfo)
{
    FN("VulkanThinCommandList::VBeginRendering");

    std::vector<VkRenderingAttachmentInfo> colorAttachments;
    colorAttachments.reserve(renderingInfo.colorAttachments.size());

    for (const ThinColorAttachment &attachment : renderingInfo.colorAttachments)
    {
        VulkanThinTexture *texture = static_cast<VulkanThinTexture *>(attachment.texture.get());
        if (texture == nullptr)
        {
            continue;
        }

        VkRenderingAttachmentInfo info = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        info.imageView = texture->GetView();
        info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        info.loadOp = VulkanThinTypes::ToLoadOp(attachment.loadOp);
        info.storeOp = VulkanThinTypes::ToStoreOp(attachment.storeOp);
        std::memcpy(info.clearValue.color.float32, attachment.clearColor, sizeof(float) * 4);
        colorAttachments.push_back(info);
    }

    VkRenderingAttachmentInfo depthAttachment = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    const bool hasDepth = (renderingInfo.depthAttachment.texture != nullptr);
    if (hasDepth)
    {
        VulkanThinTexture *texture = static_cast<VulkanThinTexture *>(renderingInfo.depthAttachment.texture.get());
        depthAttachment.imageView = texture->GetView();
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VulkanThinTypes::ToLoadOp(renderingInfo.depthAttachment.loadOp);
        depthAttachment.storeOp = VulkanThinTypes::ToStoreOp(renderingInfo.depthAttachment.storeOp);
        depthAttachment.clearValue.depthStencil.depth = renderingInfo.depthAttachment.clearDepth;
        depthAttachment.clearValue.depthStencil.stencil = renderingInfo.depthAttachment.clearStencil;
    }

    VkRenderingInfo info = {VK_STRUCTURE_TYPE_RENDERING_INFO};
    info.renderArea.extent.width = renderingInfo.width;
    info.renderArea.extent.height = renderingInfo.height;
    info.layerCount = 1;
    info.colorAttachmentCount = (uint32_t)colorAttachments.size();
    info.pColorAttachments = colorAttachments.data();
    info.pDepthAttachment = hasDepth ? &depthAttachment : nullptr;

    vkCmdBeginRendering(m_commandBuffer, &info);
}

void VulkanThinCommandList::VEndRendering() { vkCmdEndRendering(m_commandBuffer); }

void VulkanThinCommandList::VSetViewport(const ThinViewport &viewport)
{
    // A negative height flips Y, which puts Vulkan's clip space the same way up
    // as DirectX's so that one shader works on both.
    VkViewport vkViewport = {};
    vkViewport.x = viewport.x;
    vkViewport.y = viewport.y + viewport.height;
    vkViewport.width = viewport.width;
    vkViewport.height = -viewport.height;
    vkViewport.minDepth = viewport.minDepth;
    vkViewport.maxDepth = viewport.maxDepth;

    vkCmdSetViewport(m_commandBuffer, 0, 1, &vkViewport);
}

void VulkanThinCommandList::VSetScissor(const ThinScissor &scissor)
{
    VkRect2D rect = {};
    rect.offset.x = scissor.x;
    rect.offset.y = scissor.y;
    rect.extent.width = scissor.width;
    rect.extent.height = scissor.height;

    vkCmdSetScissor(m_commandBuffer, 0, 1, &rect);
}

void VulkanThinCommandList::VBindPipeline(const ThinPipelinePtr &pipeline)
{
    FN("VulkanThinCommandList::VBindPipeline");

    VulkanThinPipeline *vulkanPipeline = static_cast<VulkanThinPipeline *>(pipeline.get());
    if (vulkanPipeline == nullptr)
    {
        return;
    }

    m_boundBindPoint = vulkanPipeline->GetBindPoint();
    m_boundLayout = static_cast<VulkanThinPipelineLayout *>(vulkanPipeline->VGetLayout().get())->GetHandle();

    vkCmdBindPipeline(m_commandBuffer, m_boundBindPoint, vulkanPipeline->GetHandle());
}

void VulkanThinCommandList::VBindDescriptorSet(uint32_t setIndex, const ThinDescriptorSetPtr &descriptorSet)
{
    FN("VulkanThinCommandList::VBindDescriptorSet");

    if (m_boundLayout == VK_NULL_HANDLE)
    {
        LOG_ERROR("A descriptor set was bound before any pipeline, so there is no layout to bind it against.");
        return;
    }

    VkDescriptorSet set = static_cast<VulkanThinDescriptorSet *>(descriptorSet.get())->GetHandle();
    vkCmdBindDescriptorSets(m_commandBuffer, m_boundBindPoint, m_boundLayout, setIndex, 1, &set, 0, nullptr);
}

void VulkanThinCommandList::VSetRootConstants(ThinShaderStage stages, uint32_t offsetInBytes, uint32_t sizeInBytes, const void *data)
{
    if (m_boundLayout == VK_NULL_HANDLE)
    {
        LOG_ERROR("Root constants were set before any pipeline was bound.");
        return;
    }

    vkCmdPushConstants(m_commandBuffer, m_boundLayout, VulkanThinTypes::ToShaderStageFlags(stages), offsetInBytes, sizeInBytes, data);
}

void VulkanThinCommandList::VBindVertexBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes)
{
    VkBuffer handle = static_cast<VulkanThinBuffer *>(buffer.get())->GetHandle();
    vkCmdBindVertexBuffers(m_commandBuffer, binding, 1, &handle, &offsetInBytes);
}

void VulkanThinCommandList::VBindIndexBuffer(const ThinBufferPtr &buffer, ThinFormat indexFormat, uint64_t offsetInBytes)
{
    const VkIndexType type = (indexFormat == ThinFormat::R16Uint) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(m_commandBuffer, static_cast<VulkanThinBuffer *>(buffer.get())->GetHandle(), offsetInBytes, type);
}

void VulkanThinCommandList::VDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) { vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance); }

void VulkanThinCommandList::VDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanThinCommandList::VDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ); }

void VulkanThinCommandList::VTraceRays(uint32_t, uint32_t, uint32_t)
{
    FN("VulkanThinCommandList::VTraceRays");

    LOG_ERROR("Ray tracing is not wired up in the thin Vulkan backend yet.");
}

void VulkanThinCommandList::VCopyBuffer(const ThinBufferPtr &source, const ThinBufferPtr &destination, uint64_t sourceOffset, uint64_t destinationOffset, uint64_t sizeInBytes)
{
    VkBufferCopy region = {};
    region.srcOffset = sourceOffset;
    region.dstOffset = destinationOffset;
    region.size = sizeInBytes;

    vkCmdCopyBuffer(m_commandBuffer, static_cast<VulkanThinBuffer *>(source.get())->GetHandle(), static_cast<VulkanThinBuffer *>(destination.get())->GetHandle(), 1, &region);
}

void VulkanThinCommandList::VCopyBufferToTexture(const ThinBufferPtr &source, const ThinTexturePtr &destination, uint32_t mipLevel)
{
    VulkanThinTexture *texture = static_cast<VulkanThinTexture *>(destination.get());

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = mipLevel;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = texture->VGetDescription().width;
    region.imageExtent.height = texture->VGetDescription().height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(m_commandBuffer, static_cast<VulkanThinBuffer *>(source.get())->GetHandle(), texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void VulkanThinCommandList::VBarrier(const std::vector<ThinBarrier> &barriers)
{
    FN("VulkanThinCommandList::VBarrier");

    std::vector<VkImageMemoryBarrier2> imageBarriers;
    std::vector<VkBufferMemoryBarrier2> bufferBarriers;

    for (const ThinBarrier &barrier : barriers)
    {
        if (barrier.texture != nullptr)
        {
            VulkanThinTexture *texture = static_cast<VulkanThinTexture *>(barrier.texture.get());
            const bool depth = texture->VGetDescription().depthStencil;

            VkImageMemoryBarrier2 imageBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
            imageBarrier.srcStageMask = VulkanThinTypes::ToStageFlags(barrier.before);
            imageBarrier.srcAccessMask = VulkanThinTypes::ToAccessFlags(barrier.before);
            imageBarrier.dstStageMask = VulkanThinTypes::ToStageFlags(barrier.after);
            imageBarrier.dstAccessMask = VulkanThinTypes::ToAccessFlags(barrier.after);
            imageBarrier.oldLayout = VulkanThinTypes::ToImageLayout(barrier.before);
            imageBarrier.newLayout = VulkanThinTypes::ToImageLayout(barrier.after);
            imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imageBarrier.image = texture->GetImage();
            imageBarrier.subresourceRange.aspectMask = depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
            imageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
            imageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

            imageBarriers.push_back(imageBarrier);
        }
        else if (barrier.buffer != nullptr)
        {
            VkBufferMemoryBarrier2 bufferBarrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2};
            bufferBarrier.srcStageMask = VulkanThinTypes::ToStageFlags(barrier.before);
            bufferBarrier.srcAccessMask = VulkanThinTypes::ToAccessFlags(barrier.before);
            bufferBarrier.dstStageMask = VulkanThinTypes::ToStageFlags(barrier.after);
            bufferBarrier.dstAccessMask = VulkanThinTypes::ToAccessFlags(barrier.after);
            bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            bufferBarrier.buffer = static_cast<VulkanThinBuffer *>(barrier.buffer.get())->GetHandle();
            bufferBarrier.size = VK_WHOLE_SIZE;

            bufferBarriers.push_back(bufferBarrier);
        }
    }

    if (imageBarriers.empty() && bufferBarriers.empty())
    {
        return;
    }

    VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    dependency.imageMemoryBarrierCount = (uint32_t)imageBarriers.size();
    dependency.pImageMemoryBarriers = imageBarriers.data();
    dependency.bufferMemoryBarrierCount = (uint32_t)bufferBarriers.size();
    dependency.pBufferMemoryBarriers = bufferBarriers.data();

    vkCmdPipelineBarrier2(m_commandBuffer, &dependency);
}

void VulkanThinCommandList::VBeginDebugLabel(const char *name)
{
    PFN_vkCmdBeginDebugUtilsLabelEXT begin = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_device->GetInstance(), "vkCmdBeginDebugUtilsLabelEXT");
    if (begin != nullptr)
    {
        VkDebugUtilsLabelEXT label = {VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT};
        label.pLabelName = name;
        begin(m_commandBuffer, &label);
    }
}

void VulkanThinCommandList::VEndDebugLabel()
{
    PFN_vkCmdEndDebugUtilsLabelEXT end = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_device->GetInstance(), "vkCmdEndDebugUtilsLabelEXT");
    if (end != nullptr)
    {
        end(m_commandBuffer);
    }
}

// ================================================================ command pool

VulkanThinCommandPool::VulkanThinCommandPool(VulkanThinDevice *device, uint32_t queueFamily) : m_device(device), m_pool(VK_NULL_HANDLE)
{
    FN("VulkanThinCommandPool::VulkanThinCommandPool");

    VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamily;

    VulkanCheck(vkCreateCommandPool(device->GetHandle(), &createInfo, nullptr, &m_pool), "vkCreateCommandPool");
}

VulkanThinCommandPool::~VulkanThinCommandPool()
{
    if (m_pool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_device->GetHandle(), m_pool, nullptr);
    }
}

ThinCommandListPtr VulkanThinCommandPool::VAllocate()
{
    FN("VulkanThinCommandPool::VAllocate");

    VkCommandBufferAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocateInfo.commandPool = m_pool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    if (!VulkanCheck(vkAllocateCommandBuffers(m_device->GetHandle(), &allocateInfo, &commandBuffer), "vkAllocateCommandBuffers"))
    {
        return nullptr;
    }

    return std::make_shared<VulkanThinCommandList>(m_device, commandBuffer);
}

void VulkanThinCommandPool::VReset()
{
    FN("VulkanThinCommandPool::VReset");

    vkResetCommandPool(m_device->GetHandle(), m_pool, 0);
}

} // namespace bow
