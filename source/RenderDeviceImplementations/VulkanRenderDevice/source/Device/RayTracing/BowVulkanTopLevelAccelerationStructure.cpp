#include <VulkanRenderDevice/Device/RayTracing/BowVulkanTopLevelAccelerationStructure.h>

#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(VulkanLogicalDevice *logicalDevice)
    : m_logicalDevice(logicalDevice), m_accelerationStructure(VK_NULL_HANDLE), m_buffer(nullptr), m_instancesBuffer(nullptr), m_deviceAddress(0)
{
    FN("VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure");
}

VulkanTopLevelAccelerationStructure::~VulkanTopLevelAccelerationStructure()
{
    FN("VulkanTopLevelAccelerationStructure::~VulkanTopLevelAccelerationStructure");
    Release();
}

bool VulkanTopLevelAccelerationStructure::Initialize(VkAccelerationStructureKHR accelerationStructure, VulkanBufferPtr buffer, VulkanBufferPtr instancesBuffer)
{
    FN("VulkanTopLevelAccelerationStructure::Initialize");

    m_accelerationStructure = accelerationStructure;
    m_buffer = buffer;
    m_instancesBuffer = instancesBuffer;

    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = m_accelerationStructure;
    m_deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(m_logicalDevice->GetHandle(), &addressInfo);

    LOG_TRACE("TLAS device address: %llu", m_deviceAddress);

    return true;
}

void VulkanTopLevelAccelerationStructure::Release()
{
    FN("VulkanTopLevelAccelerationStructure::Release");

    if (m_accelerationStructure != VK_NULL_HANDLE && m_logicalDevice != nullptr)
    {
        vkDestroyAccelerationStructureKHR(m_logicalDevice->GetHandle(), m_accelerationStructure, nullptr);
        m_accelerationStructure = VK_NULL_HANDLE;
    }

    m_buffer = nullptr;
    m_instancesBuffer = nullptr;
}

} // namespace bow
