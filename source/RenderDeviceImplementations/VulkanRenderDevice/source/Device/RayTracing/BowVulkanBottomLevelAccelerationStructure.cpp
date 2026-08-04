#include <VulkanRenderDevice/Device/RayTracing/BowVulkanBottomLevelAccelerationStructure.h>

#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

VulkanBottomLevelAccelerationStructure::VulkanBottomLevelAccelerationStructure(VulkanLogicalDevice *logicalDevice)
    : m_logicalDevice(logicalDevice), m_accelerationStructure(VK_NULL_HANDLE), m_buffer(nullptr), m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_deviceAddress(0)
{
    FN("VulkanBottomLevelAccelerationStructure::VulkanBottomLevelAccelerationStructure");
}

VulkanBottomLevelAccelerationStructure::~VulkanBottomLevelAccelerationStructure()
{
    FN("VulkanBottomLevelAccelerationStructure::~VulkanBottomLevelAccelerationStructure");
    Release();
}

bool VulkanBottomLevelAccelerationStructure::Initialize(VkAccelerationStructureKHR accelerationStructure, VulkanBufferPtr buffer, VulkanBufferPtr vertexBuffer, VulkanBufferPtr indexBuffer)
{
    FN("VulkanBottomLevelAccelerationStructure::Initialize");

    m_accelerationStructure = accelerationStructure;
    m_buffer = buffer;
    m_vertexBuffer = vertexBuffer;
    m_indexBuffer = indexBuffer;

    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = m_accelerationStructure;
    m_deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(m_logicalDevice->GetHandle(), &addressInfo);

    LOG_TRACE("BLAS device address: %llu", m_deviceAddress);

    return true;
}

void VulkanBottomLevelAccelerationStructure::Release()
{
    FN("VulkanBottomLevelAccelerationStructure::Release");

    if (m_accelerationStructure != VK_NULL_HANDLE && m_logicalDevice != nullptr)
    {
        vkDestroyAccelerationStructureKHR(m_logicalDevice->GetHandle(), m_accelerationStructure, nullptr);
        m_accelerationStructure = VK_NULL_HANDLE;
    }

    m_buffer = nullptr;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
}

} // namespace bow
