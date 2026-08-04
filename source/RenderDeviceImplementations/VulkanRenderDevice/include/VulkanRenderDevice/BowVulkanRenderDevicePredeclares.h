#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace bow
{
class VulkanPhysicalDevice;
typedef std::shared_ptr<VulkanPhysicalDevice> VulkanPhysicalDevicePtr;

class VulkanLogicalDevice;
typedef std::shared_ptr<VulkanLogicalDevice> VulkanLogicalDevicePtr;

class VulkanQueueFamily;
typedef std::shared_ptr<VulkanQueueFamily> VulkanQueueFamilyPtr;

class VulkanRenderDevice;
typedef std::shared_ptr<VulkanRenderDevice> VulkanRenderDevicePtr;
typedef std::unordered_map<uint32_t, VulkanRenderDevice> VulkanRenderDeviceMap;

class VulkanInstance;
typedef std::shared_ptr<VulkanInstance> VulkanInstancePtr;
typedef std::unordered_map<uint32_t, VulkanInstance> VulkanInstanceMap;

class VulkanGraphicsWindow;
typedef std::shared_ptr<VulkanGraphicsWindow> VulkanGraphicsWindowPtr;
typedef std::unordered_map<uint32_t, VulkanGraphicsWindowPtr> VulkanGraphicsWindowMap;

class VulkanRenderSurface;
typedef std::shared_ptr<VulkanRenderSurface> VulkanRenderSurfacePtr;

class VulkanSwapchain;
typedef std::shared_ptr<VulkanSwapchain> VulkanSwapchainPtr;

class VulkanShaderProgram;
typedef std::shared_ptr<VulkanShaderProgram> VulkanShaderProgramPtr;

class VulkanRayTracingShaderProgram;
typedef std::shared_ptr<VulkanRayTracingShaderProgram> VulkanRayTracingShaderProgramPtr;

class VulkanShaderResourceBindings;
typedef std::shared_ptr<VulkanShaderResourceBindings> VulkanShaderResourceBindingsPtr;

class VulkanFragmentOutputs;
typedef std::shared_ptr<VulkanFragmentOutputs> VulkanFragmentOutputsPtr;

class VulkanPipeline;
typedef std::shared_ptr<VulkanPipeline> VulkanPipelinePtr;

class VulkanShaderResource;
typedef std::shared_ptr<VulkanShaderResource> VulkanShaderResourcePtr;
typedef std::unordered_map<uint32_t, VulkanShaderResourcePtr> VulkanShaderResourceMap;

class VulkanPushConstant;
typedef std::shared_ptr<VulkanPushConstant> VulkanPushConstantPtr;
typedef std::unordered_map<uint32_t, VulkanPushConstantPtr> VulkanPushConstantMap;

class VulkanTexture2D;
typedef std::shared_ptr<VulkanTexture2D> VulkanTexture2DPtr;

class VulkanTextureSampler;
typedef std::shared_ptr<VulkanTextureSampler> VulkanTextureSamplerPtr;

class VulkanColorAttachments;
typedef std::shared_ptr<VulkanColorAttachments> VulkanColorAttachmentsPtr;

class VulkanColorAttachment;
typedef std::shared_ptr<VulkanColorAttachment> VulkanColorAttachmentPtr;
typedef std::unordered_map<uint32_t, VulkanColorAttachment> VulkanColorAttachmentMap;

class VulkanRenderPass;
typedef std::shared_ptr<VulkanRenderPass> VulkanRenderPassPtr;

class VulkanFramebuffer;
typedef std::shared_ptr<VulkanFramebuffer> VulkanFramebufferPtr;

class VulkanDeviceMemory;
typedef std::shared_ptr<VulkanDeviceMemory> VulkanDeviceMemoryPtr;

class VulkanBuffer;
typedef std::shared_ptr<VulkanBuffer> VulkanBufferPtr;

class VulkanVertexAttributeBindings;
typedef std::shared_ptr<VulkanVertexAttributeBindings> VulkanVertexAttributeBindingsPtr;

class VulkanVertexBuffer;
typedef std::shared_ptr<VulkanVertexBuffer> VulkanVertexBufferPtr;

class VulkanIndexBuffer;
typedef std::shared_ptr<VulkanIndexBuffer> VulkanIndexBufferPtr;

class VulkanUniformBuffer;
typedef std::shared_ptr<VulkanUniformBuffer> VulkanUniformBufferPtr;

class VulkanStorageBuffer;
typedef std::shared_ptr<VulkanStorageBuffer> VulkanStorageBufferPtr;

class VulkanCommandPool;
typedef std::shared_ptr<VulkanCommandPool> VulkanCommandPoolPtr;

class VulkanCommandBuffer;
typedef std::shared_ptr<VulkanCommandBuffer> VulkanCommandBufferPtr;

class VulkanSingleUseCommandBuffer;
typedef std::shared_ptr<VulkanSingleUseCommandBuffer> VulkanSingleUseCommandBufferPtr;

class VulkanSemaphore;
typedef std::shared_ptr<VulkanSemaphore> VulkanSemaphorePtr;

class VulkanFence;
typedef std::shared_ptr<VulkanFence> VulkanFencePtr;

} // namespace bow