#pragma once
#include <ThinRenderDevice/BowThinPipeline.h>
#include <ThinRenderDevice/BowThinRenderDevicePredeclares.h>
#include <ThinRenderDevice/ThinRenderDevice_api.h>

namespace bow
{

//! A resource changing role, which both APIs must be told about explicitly.
struct ThinBarrier
{
    ThinBufferPtr buffer;
    ThinTexturePtr texture;

    ThinResourceState before = ThinResourceState::Undefined;
    ThinResourceState after = ThinResourceState::General;
};

//! What happens to an attachment when rendering begins.
enum class ThinLoadOp : char
{
    Load,
    Clear,
    DontCare
};

//! What happens to it when rendering ends.
enum class ThinStoreOp : char
{
    Store,
    DontCare
};

struct ThinColorAttachment
{
    ThinTexturePtr texture;
    ThinLoadOp loadOp = ThinLoadOp::Clear;
    ThinStoreOp storeOp = ThinStoreOp::Store;
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};

struct ThinDepthAttachment
{
    ThinTexturePtr texture;
    ThinLoadOp loadOp = ThinLoadOp::Clear;
    ThinStoreOp storeOp = ThinStoreOp::Store;
    float clearDepth = 1.0f;
    uint8_t clearStencil = 0;
};

//! The attachments a pass renders into.
/*!
    This is dynamic rendering: no render pass object, no framebuffer object,
    just the targets stated when the pass begins. Vulkan 1.3 has it in core and
    DirectX 12 has always worked this way.
*/
struct ThinRenderingInfo
{
    std::vector<ThinColorAttachment> colorAttachments;
    ThinDepthAttachment depthAttachment;

    uint32_t width = 0;
    uint32_t height = 0;
};

struct ThinViewport
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

struct ThinScissor
{
    int32_t x = 0;
    int32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

//! Commands recorded for the GPU.
/*!
    Nothing here executes when it is called; the list is submitted to a queue.
    That is the whole difference from the classic tier, where the driver
    decides when work is sent.
*/
class IThinCommandList
{
  public:
    virtual ~IThinCommandList() {}

    virtual void VBegin() = 0;
    virtual void VEnd() = 0;

    virtual void VBeginRendering(const ThinRenderingInfo &renderingInfo) = 0;
    virtual void VEndRendering() = 0;

    virtual void VSetViewport(const ThinViewport &viewport) = 0;
    virtual void VSetScissor(const ThinScissor &scissor) = 0;

    virtual void VBindPipeline(const ThinPipelinePtr &pipeline) = 0;
    virtual void VBindDescriptorSet(uint32_t setIndex, const ThinDescriptorSetPtr &descriptorSet) = 0;
    virtual void VSetRootConstants(ThinShaderStage stages, uint32_t offsetInBytes, uint32_t sizeInBytes, const void *data) = 0;

    virtual void VBindVertexBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes = 0) = 0;
    virtual void VBindIndexBuffer(const ThinBufferPtr &buffer, ThinFormat indexFormat, uint64_t offsetInBytes = 0) = 0;

    virtual void VDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void VDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;

    virtual void VDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    virtual void VTraceRays(uint32_t width, uint32_t height, uint32_t depth) = 0;

    virtual void VCopyBuffer(const ThinBufferPtr &source, const ThinBufferPtr &destination, uint64_t sourceOffset, uint64_t destinationOffset, uint64_t sizeInBytes) = 0;
    virtual void VCopyBufferToTexture(const ThinBufferPtr &source, const ThinTexturePtr &destination, uint32_t mipLevel = 0) = 0;

    //! Announces resources changing role. Nothing else infers these.
    virtual void VBarrier(const std::vector<ThinBarrier> &barriers) = 0;

    //! Marks a region in a capture, which both APIs surface to their tools.
    virtual void VBeginDebugLabel(const char *name) = 0;
    virtual void VEndDebugLabel() = 0;
};

//! Allocates command lists. Resetting one recycles every list it handed out.
class IThinCommandPool
{
  public:
    virtual ~IThinCommandPool() {}

    virtual ThinCommandListPtr VAllocate() = 0;
    virtual void VReset() = 0;
};

//! A counter the GPU raises and the CPU can wait on.
/*!
    A Vulkan timeline semaphore and a DirectX 12 fence are the same object, so
    the thin tier has only this one. Binary semaphores exist in Vulkan solely
    for swapchain acquire, which IThinSwapchain hides.
*/
class IThinFence
{
  public:
    virtual ~IThinFence() {}

    virtual uint64_t VGetCompletedValue() const = 0;
    virtual void VWait(uint64_t value) = 0;
    virtual void VSignal(uint64_t value) = 0;
};

//! Where recorded work is submitted.
class IThinQueue
{
  public:
    virtual ~IThinQueue() {}

    virtual ThinQueueType VGetType() const = 0;

    //! Submits work, waiting on and signalling fence values.
    virtual void VSubmit(const std::vector<ThinCommandListPtr> &commandLists, const ThinFencePtr &waitFence = nullptr, uint64_t waitValue = 0, const ThinFencePtr &signalFence = nullptr, uint64_t signalValue = 0) = 0;

    //! Blocks until everything submitted so far has finished.
    virtual void VWaitIdle() = 0;
};

} // namespace bow
