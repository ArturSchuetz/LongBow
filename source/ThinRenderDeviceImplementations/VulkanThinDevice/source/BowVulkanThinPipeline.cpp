#include <VulkanThinDevice/BowVulkanThinObjects.h>

#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

// ============================================================ pipeline layout

VulkanThinPipelineLayout::VulkanThinPipelineLayout(VulkanThinDevice *device, const ThinPipelineLayoutDescription &description) : m_device(device), m_description(description), m_layout(VK_NULL_HANDLE)
{
    FN("VulkanThinPipelineLayout::VulkanThinPipelineLayout");

    for (const ThinDescriptorSetLayoutDescription &set : description.descriptorSets)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(set.bindings.size());

        for (const ThinDescriptorBinding &binding : set.bindings)
        {
            VkDescriptorSetLayoutBinding vkBinding = {};
            vkBinding.binding = binding.binding;
            vkBinding.descriptorType = VulkanThinTypes::ToDescriptorType(binding.type);
            vkBinding.descriptorCount = binding.count;
            vkBinding.stageFlags = VulkanThinTypes::ToShaderStageFlags(binding.stages);
            bindings.push_back(vkBinding);
        }

        VkDescriptorSetLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        createInfo.bindingCount = (uint32_t)bindings.size();
        createInfo.pBindings = bindings.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        if (VulkanCheck(vkCreateDescriptorSetLayout(device->GetHandle(), &createInfo, nullptr, &layout), "vkCreateDescriptorSetLayout"))
        {
            m_setLayouts.push_back(layout);
        }
    }

    std::vector<VkPushConstantRange> pushConstants;
    pushConstants.reserve(description.rootConstants.size());
    for (const ThinRootConstantRange &range : description.rootConstants)
    {
        VkPushConstantRange vkRange = {};
        vkRange.stageFlags = VulkanThinTypes::ToShaderStageFlags(range.stages);
        vkRange.offset = range.offsetInBytes;
        vkRange.size = range.sizeInBytes;
        pushConstants.push_back(vkRange);
    }

    VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    createInfo.setLayoutCount = (uint32_t)m_setLayouts.size();
    createInfo.pSetLayouts = m_setLayouts.data();
    createInfo.pushConstantRangeCount = (uint32_t)pushConstants.size();
    createInfo.pPushConstantRanges = pushConstants.data();

    VulkanCheck(vkCreatePipelineLayout(device->GetHandle(), &createInfo, nullptr, &m_layout), "vkCreatePipelineLayout");
}

VulkanThinPipelineLayout::~VulkanThinPipelineLayout()
{
    for (VkDescriptorSetLayout layout : m_setLayouts)
    {
        vkDestroyDescriptorSetLayout(m_device->GetHandle(), layout, nullptr);
    }
    if (m_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device->GetHandle(), m_layout, nullptr);
    }
}

// ==================================================================== pipeline

VulkanThinPipeline::VulkanThinPipeline(VulkanThinDevice *device, VkPipeline pipeline, VkPipelineBindPoint bindPoint, const ThinPipelineLayoutPtr &layout) : m_device(device), m_pipeline(pipeline), m_bindPoint(bindPoint), m_layout(layout) {}

VulkanThinPipeline::~VulkanThinPipeline()
{
    if (m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device->GetHandle(), m_pipeline, nullptr);
    }
}

// ============================================================== descriptor set

VulkanThinDescriptorSet::VulkanThinDescriptorSet(VulkanThinDevice *device, VkDescriptorSet set) : m_device(device), m_set(set) {}

VulkanThinDescriptorSet::~VulkanThinDescriptorSet() {}

void VulkanThinDescriptorSet::VSetBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes, uint64_t sizeInBytes)
{
    FN("VulkanThinDescriptorSet::VSetBuffer");

    VulkanThinBuffer *vulkanBuffer = static_cast<VulkanThinBuffer *>(buffer.get());
    if (vulkanBuffer == nullptr)
    {
        return;
    }

    PendingWrite write = {};
    write.binding = binding;
    write.type = vulkanBuffer->VGetDescription().storageBuffer ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.bufferInfo.buffer = vulkanBuffer->GetHandle();
    write.bufferInfo.offset = offsetInBytes;
    write.bufferInfo.range = (sizeInBytes > 0) ? sizeInBytes : VK_WHOLE_SIZE;

    m_pending.push_back(write);
}

void VulkanThinDescriptorSet::VSetTexture(uint32_t binding, const ThinTexturePtr &texture)
{
    FN("VulkanThinDescriptorSet::VSetTexture");

    VulkanThinTexture *vulkanTexture = static_cast<VulkanThinTexture *>(texture.get());
    if (vulkanTexture == nullptr)
    {
        return;
    }

    PendingWrite write = {};
    write.binding = binding;
    write.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.imageInfo.imageView = vulkanTexture->GetView();
    write.imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    m_pending.push_back(write);
}

void VulkanThinDescriptorSet::VSetStorageTexture(uint32_t binding, const ThinTexturePtr &texture)
{
    FN("VulkanThinDescriptorSet::VSetStorageTexture");

    VulkanThinTexture *vulkanTexture = static_cast<VulkanThinTexture *>(texture.get());
    if (vulkanTexture == nullptr)
    {
        return;
    }

    PendingWrite write = {};
    write.binding = binding;
    write.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write.imageInfo.imageView = vulkanTexture->GetView();
    write.imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    m_pending.push_back(write);
}

void VulkanThinDescriptorSet::VSetSampler(uint32_t binding, const ThinSamplerPtr &sampler)
{
    FN("VulkanThinDescriptorSet::VSetSampler");

    VulkanThinSampler *vulkanSampler = static_cast<VulkanThinSampler *>(sampler.get());
    if (vulkanSampler == nullptr)
    {
        return;
    }

    // A sampler set on its own is a plain sampler descriptor; one set together
    // with a texture is folded into that texture's combined descriptor below.
    for (PendingWrite &write : m_pending)
    {
        if (write.binding == binding && write.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        {
            write.imageInfo.sampler = vulkanSampler->GetHandle();
            return;
        }
    }

    PendingWrite write = {};
    write.binding = binding;
    write.type = VK_DESCRIPTOR_TYPE_SAMPLER;
    write.imageInfo.sampler = vulkanSampler->GetHandle();

    m_pending.push_back(write);
}

void VulkanThinDescriptorSet::VSetAccelerationStructure(uint32_t, const ThinAccelerationStructurePtr &)
{
    FN("VulkanThinDescriptorSet::VSetAccelerationStructure");

    LOG_ERROR("Ray tracing is not wired up in the thin Vulkan backend yet.");
}

void VulkanThinDescriptorSet::VUpdate()
{
    FN("VulkanThinDescriptorSet::VUpdate");

    if (m_pending.empty())
    {
        return;
    }

    // The infos have to outlive the call, so the writes point into m_pending
    // rather than into temporaries.
    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(m_pending.size());

    for (PendingWrite &pending : m_pending)
    {
        VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = m_set;
        write.dstBinding = pending.binding;
        write.descriptorCount = 1;
        write.descriptorType = pending.type;

        if (pending.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || pending.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
        {
            write.pBufferInfo = &pending.bufferInfo;
        }
        else
        {
            write.pImageInfo = &pending.imageInfo;
        }

        writes.push_back(write);
    }

    vkUpdateDescriptorSets(m_device->GetHandle(), (uint32_t)writes.size(), writes.data(), 0, nullptr);
    m_pending.clear();
}

// ============================================================= descriptor pool

VulkanThinDescriptorPool::VulkanThinDescriptorPool(VulkanThinDevice *device, uint32_t maxSets) : m_device(device), m_pool(VK_NULL_HANDLE)
{
    FN("VulkanThinDescriptorPool::VulkanThinDescriptorPool");

    // Sized generously per type rather than counted exactly: a pool that runs
    // out fails allocation, and the memory involved is small.
    const VkDescriptorPoolSize sizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets * 4},   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxSets * 4}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets * 4},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxSets * 2},    {VK_DESCRIPTOR_TYPE_SAMPLER, maxSets * 2},
    };

    VkDescriptorPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = (uint32_t)(sizeof(sizes) / sizeof(sizes[0]));
    createInfo.pPoolSizes = sizes;

    VulkanCheck(vkCreateDescriptorPool(device->GetHandle(), &createInfo, nullptr, &m_pool), "vkCreateDescriptorPool");
}

VulkanThinDescriptorPool::~VulkanThinDescriptorPool()
{
    for (VkDescriptorSetLayout layout : m_setLayouts)
    {
        vkDestroyDescriptorSetLayout(m_device->GetHandle(), layout, nullptr);
    }
    if (m_pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_device->GetHandle(), m_pool, nullptr);
    }
}

ThinDescriptorSetPtr VulkanThinDescriptorPool::VAllocate(const ThinDescriptorSetLayoutDescription &layout)
{
    FN("VulkanThinDescriptorPool::VAllocate");

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(layout.bindings.size());

    for (const ThinDescriptorBinding &binding : layout.bindings)
    {
        VkDescriptorSetLayoutBinding vkBinding = {};
        vkBinding.binding = binding.binding;
        vkBinding.descriptorType = VulkanThinTypes::ToDescriptorType(binding.type);
        vkBinding.descriptorCount = binding.count;
        vkBinding.stageFlags = VulkanThinTypes::ToShaderStageFlags(binding.stages);
        bindings.push_back(vkBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    layoutInfo.bindingCount = (uint32_t)bindings.size();
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
    if (!VulkanCheck(vkCreateDescriptorSetLayout(m_device->GetHandle(), &layoutInfo, nullptr, &setLayout), "vkCreateDescriptorSetLayout"))
    {
        return nullptr;
    }
    m_setLayouts.push_back(setLayout);

    VkDescriptorSetAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    allocateInfo.descriptorPool = m_pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &setLayout;

    VkDescriptorSet set = VK_NULL_HANDLE;
    if (!VulkanCheck(vkAllocateDescriptorSets(m_device->GetHandle(), &allocateInfo, &set), "vkAllocateDescriptorSets"))
    {
        return nullptr;
    }

    return std::make_shared<VulkanThinDescriptorSet>(m_device, set);
}

void VulkanThinDescriptorPool::VReset()
{
    FN("VulkanThinDescriptorPool::VReset");

    vkResetDescriptorPool(m_device->GetHandle(), m_pool, 0);
}

} // namespace bow
