#pragma once
#include <ThinRenderDevice/BowThinCommandList.h>
#include <ThinRenderDevice/BowThinPipeline.h>
#include <ThinRenderDevice/BowThinRenderDevicePredeclares.h>
#include <ThinRenderDevice/BowThinResources.h>
#include <ThinRenderDevice/ThinRenderDevice_api.h>

namespace bow
{

//! Images the swapchain hands out, one at a time.
class IThinSwapchain
{
  public:
    virtual ~IThinSwapchain() {}

    //! Takes the next image, blocking until it is free to render into.
    virtual uint32_t VAcquireNextImage() = 0;

    virtual ThinTexturePtr VGetImage(uint32_t index) const = 0;
    virtual uint32_t VGetImageCount() const = 0;
    virtual ThinFormat VGetFormat() const = 0;

    virtual uint32_t VGetWidth() const = 0;
    virtual uint32_t VGetHeight() const = 0;

    //! Presents the acquired image.
    virtual void VPresent(bool vsync = false) = 0;

    //! Rebuilds the images after the window changed size.
    virtual void VResize(uint32_t width, uint32_t height) = 0;
};

//! What the device can do, so callers can decide rather than crash.
struct ThinDeviceCapabilities
{
    bool rayTracing = false;
    bool meshShaders = false;
    bool bindlessResources = false;
    uint32_t maxRootConstantBytes = 128;
    std::string adapterName;
};

//! An explicit graphics device: Vulkan or DirectX 12.
/*!
    Everything is created up front and recorded into command lists, which is
    what these APIs are for. The classic IRenderDevice sits alongside this for
    OpenGL and DirectX 11, whose model is different enough that sharing one
    interface made both worse -- see docs/ARCHITECTURE.md.
*/
class IThinDevice
{
  public:
    virtual ~IThinDevice() {}
    virtual void VRelease() = 0;

    virtual const ThinDeviceCapabilities &VGetCapabilities() const = 0;

    virtual ThinQueuePtr VGetQueue(ThinQueueType type) = 0;

    //! Creates a window and the swapchain presenting to it.
    virtual ThinSwapchainPtr VCreateSwapchain(void *nativeWindowHandle, uint32_t width, uint32_t height, uint32_t imageCount = 2) = 0;

    virtual ThinCommandPoolPtr VCreateCommandPool(ThinQueueType type) = 0;
    virtual ThinFencePtr VCreateFence(uint64_t initialValue = 0) = 0;

    virtual ThinBufferPtr VCreateBuffer(const ThinBufferDescription &description) = 0;
    virtual ThinTexturePtr VCreateTexture(const ThinTextureDescription &description) = 0;
    virtual ThinSamplerPtr VCreateSampler(const ThinSamplerDescription &description) = 0;

    //! Wraps compiled bytecode: SPIR-V for Vulkan, DXIL for DirectX 12.
    virtual ThinShaderModulePtr VCreateShaderModule(const void *byteCode, size_t sizeInBytes, ThinShaderStage stage, const char *entryPoint = "main") = 0;

    virtual ThinPipelineLayoutPtr VCreatePipelineLayout(const ThinPipelineLayoutDescription &description) = 0;
    virtual ThinPipelinePtr VCreateGraphicsPipeline(const ThinGraphicsPipelineDescription &description) = 0;
    virtual ThinPipelinePtr VCreateComputePipeline(const ThinComputePipelineDescription &description) = 0;
    virtual ThinPipelinePtr VCreateRayTracingPipeline(const ThinRayTracingPipelineDescription &description) = 0;

    virtual ThinDescriptorPoolPtr VCreateDescriptorPool(uint32_t maxSets) = 0;

    //! Builds the acceleration structure covering one mesh.
    virtual ThinAccelerationStructurePtr VCreateBottomLevelAccelerationStructure(const ThinBufferPtr &vertexBuffer, uint32_t vertexCount, uint32_t vertexStrideInBytes, const ThinBufferPtr &indexBuffer, uint32_t indexCount) = 0;

    //! Builds the structure holding instances of the ones above.
    virtual ThinAccelerationStructurePtr VCreateTopLevelAccelerationStructure(const std::vector<ThinAccelerationStructurePtr> &bottomLevel) = 0;

    //! Blocks until the device has finished everything.
    virtual void VWaitIdle() = 0;
};

//! An acceleration structure, and the address a shader reaches it by.
class IThinAccelerationStructure
{
  public:
    virtual ~IThinAccelerationStructure() {}

    virtual uint64_t VGetDeviceAddress() const = 0;
};

} // namespace bow
