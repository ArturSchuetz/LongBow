#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/Device/RayTracing/IBowBottomLevelAccelerationStructure.h>

namespace bow
{

class VulkanBottomLevelAccelerationStructure : public IBottomLevelAccelerationStructure
{
  public:
    VulkanBottomLevelAccelerationStructure(VulkanLogicalDevice *logicalDevice);
    virtual ~VulkanBottomLevelAccelerationStructure();

    bool Initialize(VkAccelerationStructureKHR accelerationStructure, VulkanBufferPtr buffer, VulkanBufferPtr vertexBuffer, VulkanBufferPtr indexBuffer);
    void Release();

    VkAccelerationStructureKHR GetHandle() const { return m_accelerationStructure; }
    VkDeviceAddress GetDeviceAddress() const { return m_deviceAddress; }

  private:
    VulkanLogicalDevice *m_logicalDevice;
    VkAccelerationStructureKHR m_accelerationStructure;
    VulkanBufferPtr m_buffer;
    VulkanBufferPtr m_vertexBuffer;
    VulkanBufferPtr m_indexBuffer;
    VkDeviceAddress m_deviceAddress;
};

} // namespace bow
