#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/Device/RayTracing/IBowTopLevelAccelerationStructure.h>

namespace bow
{

class VulkanTopLevelAccelerationStructure : public ITopLevelAccelerationStructure
{
  public:
    VulkanTopLevelAccelerationStructure(VulkanLogicalDevice *logicalDevice);
    virtual ~VulkanTopLevelAccelerationStructure();

    bool Initialize(VkAccelerationStructureKHR accelerationStructure, VulkanBufferPtr buffer, VulkanBufferPtr instancesBuffer);
    void Release();

    VkAccelerationStructureKHR GetHandle() const { return m_accelerationStructure; }
    VkDeviceAddress GetDeviceAddress() const { return m_deviceAddress; }

  private:
    VulkanLogicalDevice *m_logicalDevice;
    VkAccelerationStructureKHR m_accelerationStructure;
    VulkanBufferPtr m_buffer;
    VulkanBufferPtr m_instancesBuffer;
    VkDeviceAddress m_deviceAddress;
};

} // namespace bow
