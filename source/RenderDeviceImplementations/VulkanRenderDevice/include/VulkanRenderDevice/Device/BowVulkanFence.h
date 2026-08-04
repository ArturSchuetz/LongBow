#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

namespace bow
{

enum class FenceStatus : uint8_t
{
    Invalid,
    Ready,
    NotReady
};

class VulkanFence
{
  public:
    VulkanFence();
    ~VulkanFence();

    bool Initialize(VulkanLogicalDevice *logicalDevice);
    void Release();

    FenceStatus GetFenceStatus();
    bool WaitForFence();
    bool ResetFence();

    VkFence GetHandle() const { return m_fence; }

  private:
    // you shall not copy!
    VulkanFence(const VulkanFence &) = delete;
    VulkanFence &operator=(const VulkanFence &) = delete;

    VulkanLogicalDevice *m_logicalDevice;
    VkFence m_fence;

    std::string m_guid;
};

} // namespace bow