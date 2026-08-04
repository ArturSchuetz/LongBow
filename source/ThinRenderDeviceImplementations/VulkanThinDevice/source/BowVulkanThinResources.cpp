#include <VulkanThinDevice/BowVulkanThinObjects.h>

#include <VulkanThinDevice/BowVulkanThinDevice.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

// ====================================================================== buffer

VulkanThinBuffer::VulkanThinBuffer(VulkanThinDevice *device, const ThinBufferDescription &description) : m_device(device), m_description(description), m_buffer(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE), m_mapped(nullptr)
{
    FN("VulkanThinBuffer::VulkanThinBuffer");

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (description.vertexBuffer)
        usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (description.indexBuffer)
        usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (description.constantBuffer)
        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (description.storageBuffer)
        usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (description.indirectArgument)
        usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if (description.accelerationStructureInput)
        usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

    // Every buffer can report a device address. Ray tracing needs it, and it
    // costs nothing when unused.
    usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    VkBufferCreateInfo createInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    createInfo.size = description.sizeInBytes;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (!VulkanCheck(vkCreateBuffer(device->GetHandle(), &createInfo, nullptr, &m_buffer), "vkCreateBuffer"))
    {
        return;
    }

    VkMemoryRequirements requirements = {};
    vkGetBufferMemoryRequirements(device->GetHandle(), m_buffer, &requirements);

    // Host-visible memory is writable without a copy but slower for the GPU to
    // read; device-local is the other way round. The description says which
    // trade the caller wants.
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (description.cpuVisible || description.cpuReadable)
    {
        properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    const uint32_t memoryType = device->FindMemoryType(requirements.memoryTypeBits, properties);
    if (memoryType == UINT32_MAX)
    {
        LOG_ERROR("No memory type on this device satisfies the buffer's requirements.");
        return;
    }

    VkMemoryAllocateFlagsInfo flagsInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO};
    flagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    VkMemoryAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocateInfo.pNext = &flagsInfo;
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = memoryType;

    if (!VulkanCheck(vkAllocateMemory(device->GetHandle(), &allocateInfo, nullptr, &m_memory), "vkAllocateMemory"))
    {
        return;
    }

    vkBindBufferMemory(device->GetHandle(), m_buffer, m_memory, 0);
}

VulkanThinBuffer::~VulkanThinBuffer()
{
    if (m_mapped != nullptr)
    {
        VUnmap();
    }
    if (m_buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_device->GetHandle(), m_buffer, nullptr);
    }
    if (m_memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_device->GetHandle(), m_memory, nullptr);
    }
}

void *VulkanThinBuffer::VMap()
{
    FN("VulkanThinBuffer::VMap");

    if (!m_description.cpuVisible && !m_description.cpuReadable)
    {
        LOG_ERROR("This buffer is device-local and cannot be mapped. Create it with cpuVisible or copy through a staging buffer.");
        return nullptr;
    }

    if (m_mapped == nullptr)
    {
        vkMapMemory(m_device->GetHandle(), m_memory, 0, VK_WHOLE_SIZE, 0, &m_mapped);
    }
    return m_mapped;
}

void VulkanThinBuffer::VUnmap()
{
    if (m_mapped != nullptr)
    {
        vkUnmapMemory(m_device->GetHandle(), m_memory);
        m_mapped = nullptr;
    }
}

uint64_t VulkanThinBuffer::VGetDeviceAddress() const
{
    if (m_buffer == VK_NULL_HANDLE)
    {
        return 0;
    }

    VkBufferDeviceAddressInfo info = {VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
    info.buffer = m_buffer;
    return vkGetBufferDeviceAddress(m_device->GetHandle(), &info);
}

// ===================================================================== texture

VulkanThinTexture::VulkanThinTexture(VulkanThinDevice *device, const ThinTextureDescription &description)
    : m_device(device), m_description(description), m_image(VK_NULL_HANDLE), m_view(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE), m_ownsImage(true)
{
    FN("VulkanThinTexture::VulkanThinTexture");

    VkImageUsageFlags usage = 0;
    if (description.sampled)
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (description.storage)
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (description.renderTarget)
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (description.depthStencil)
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (description.copySource)
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (description.copyDestination)
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    VkImageCreateInfo createInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = VulkanThinTypes::ToVkFormat(description.format);
    createInfo.extent.width = description.width;
    createInfo.extent.height = description.height;
    createInfo.extent.depth = description.depth;
    createInfo.mipLevels = description.mipLevels;
    createInfo.arrayLayers = description.arrayLayers;
    createInfo.samples = (VkSampleCountFlagBits)description.sampleCount;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = usage;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (!VulkanCheck(vkCreateImage(device->GetHandle(), &createInfo, nullptr, &m_image), "vkCreateImage"))
    {
        return;
    }

    VkMemoryRequirements requirements = {};
    vkGetImageMemoryRequirements(device->GetHandle(), m_image, &requirements);

    const uint32_t memoryType = device->FindMemoryType(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memoryType == UINT32_MAX)
    {
        LOG_ERROR("No memory type on this device satisfies the texture's requirements.");
        return;
    }

    VkMemoryAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = memoryType;

    if (!VulkanCheck(vkAllocateMemory(device->GetHandle(), &allocateInfo, nullptr, &m_memory), "vkAllocateMemory"))
    {
        return;
    }

    vkBindImageMemory(device->GetHandle(), m_image, m_memory, 0);
    CreateView();
}

VulkanThinTexture::VulkanThinTexture(VulkanThinDevice *device, VkImage image, const ThinTextureDescription &description)
    : m_device(device), m_description(description), m_image(image), m_view(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE), m_ownsImage(false)
{
    FN("VulkanThinTexture::VulkanThinTexture");

    CreateView();
}

bool VulkanThinTexture::CreateView()
{
    VkImageViewCreateInfo createInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    createInfo.image = m_image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VulkanThinTypes::ToVkFormat(m_description.format);
    createInfo.subresourceRange.aspectMask = m_description.depthStencil ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.levelCount = m_description.mipLevels;
    createInfo.subresourceRange.layerCount = m_description.arrayLayers;

    return VulkanCheck(vkCreateImageView(m_device->GetHandle(), &createInfo, nullptr, &m_view), "vkCreateImageView");
}

VulkanThinTexture::~VulkanThinTexture()
{
    if (m_view != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_device->GetHandle(), m_view, nullptr);
    }
    // A swapchain image belongs to the swapchain; only the view is ours.
    if (m_ownsImage && m_image != VK_NULL_HANDLE)
    {
        vkDestroyImage(m_device->GetHandle(), m_image, nullptr);
    }
    if (m_memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_device->GetHandle(), m_memory, nullptr);
    }
}

// ===================================================================== sampler

VulkanThinSampler::VulkanThinSampler(VulkanThinDevice *device, const ThinSamplerDescription &description) : m_device(device), m_sampler(VK_NULL_HANDLE)
{
    FN("VulkanThinSampler::VulkanThinSampler");

    auto toAddressMode = [](ThinSamplerDescription::AddressMode mode) {
        switch (mode)
        {
        case ThinSamplerDescription::AddressMode::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case ThinSamplerDescription::AddressMode::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case ThinSamplerDescription::AddressMode::ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    };

    VkSamplerCreateInfo createInfo = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    createInfo.magFilter = description.magLinear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    createInfo.minFilter = description.minLinear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    createInfo.mipmapMode = description.mipLinear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    createInfo.addressModeU = toAddressMode(description.addressU);
    createInfo.addressModeV = toAddressMode(description.addressV);
    createInfo.addressModeW = toAddressMode(description.addressW);
    createInfo.anisotropyEnable = (description.maxAnisotropy > 1.0f) ? VK_TRUE : VK_FALSE;
    createInfo.maxAnisotropy = description.maxAnisotropy;
    createInfo.minLod = description.minLod;
    createInfo.maxLod = description.maxLod;

    VulkanCheck(vkCreateSampler(device->GetHandle(), &createInfo, nullptr, &m_sampler), "vkCreateSampler");
}

VulkanThinSampler::~VulkanThinSampler()
{
    if (m_sampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(m_device->GetHandle(), m_sampler, nullptr);
    }
}

// =============================================================== shader module

VulkanThinShaderModule::VulkanThinShaderModule(VulkanThinDevice *device, const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char *entryPoint)
    : m_device(device), m_module(VK_NULL_HANDLE), m_stage(stage), m_entryPoint(entryPoint != nullptr ? entryPoint : "main")
{
    FN("VulkanThinShaderModule::VulkanThinShaderModule");

    VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    createInfo.codeSize = sizeInBytes;
    createInfo.pCode = static_cast<const uint32_t *>(byteCode);

    VulkanCheck(vkCreateShaderModule(device->GetHandle(), &createInfo, nullptr, &m_module), "vkCreateShaderModule");
}

VulkanThinShaderModule::~VulkanThinShaderModule()
{
    if (m_module != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_device->GetHandle(), m_module, nullptr);
    }
}

// ========================================================= acceleration struct

VulkanThinAccelerationStructure::VulkanThinAccelerationStructure(VulkanThinDevice *device, VkAccelerationStructureKHR handle, VkBuffer buffer, VkDeviceMemory memory, uint64_t deviceAddress)
    : m_device(device), m_handle(handle), m_buffer(buffer), m_memory(memory), m_deviceAddress(deviceAddress)
{
}

VulkanThinAccelerationStructure::~VulkanThinAccelerationStructure()
{
    if (m_buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_device->GetHandle(), m_buffer, nullptr);
    }
    if (m_memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_device->GetHandle(), m_memory, nullptr);
    }
}

} // namespace bow
