#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace bow
{

class IThinDevice;
class IThinQueue;
class IThinCommandPool;
class IThinCommandList;
class IThinFence;
class IThinSwapchain;
class IThinBuffer;
class IThinTexture;
class IThinSampler;
class IThinShaderModule;
class IThinPipeline;
class IThinPipelineLayout;
class IThinDescriptorPool;
class IThinDescriptorSet;
class IThinAccelerationStructure;

typedef std::shared_ptr<IThinDevice> ThinDevicePtr;
typedef std::shared_ptr<IThinQueue> ThinQueuePtr;
typedef std::shared_ptr<IThinCommandPool> ThinCommandPoolPtr;
typedef std::shared_ptr<IThinCommandList> ThinCommandListPtr;
typedef std::shared_ptr<IThinFence> ThinFencePtr;
typedef std::shared_ptr<IThinSwapchain> ThinSwapchainPtr;
typedef std::shared_ptr<IThinBuffer> ThinBufferPtr;
typedef std::shared_ptr<IThinTexture> ThinTexturePtr;
typedef std::shared_ptr<IThinSampler> ThinSamplerPtr;
typedef std::shared_ptr<IThinShaderModule> ThinShaderModulePtr;
typedef std::shared_ptr<IThinPipeline> ThinPipelinePtr;
typedef std::shared_ptr<IThinPipelineLayout> ThinPipelineLayoutPtr;
typedef std::shared_ptr<IThinDescriptorPool> ThinDescriptorPoolPtr;
typedef std::shared_ptr<IThinDescriptorSet> ThinDescriptorSetPtr;
typedef std::shared_ptr<IThinAccelerationStructure> ThinAccelerationStructurePtr;

//! Which explicit API a device talks to.
enum class ThinRenderDeviceAPI : char
{
    DirectX12,
    Vulkan
};

//! What a resource is currently being used for.
/*!
    Both APIs need to be told when a resource changes role, and neither can
    work it out on its own. One enum covers both: Vulkan turns it into an image
    layout plus access and stage flags, DirectX 12 into a D3D12_RESOURCE_STATES
    value. Barriers are therefore part of this interface rather than something
    a backend guesses at.
*/
enum class ThinResourceState : uint32_t
{
    Undefined,
    General,
    VertexBuffer,
    IndexBuffer,
    ConstantBuffer,
    IndirectArgument,
    ShaderResource,
    UnorderedAccess,
    RenderTarget,
    DepthWrite,
    DepthRead,
    CopySource,
    CopyDestination,
    Present,
    AccelerationStructure
};

//! Which queue a piece of work belongs on.
enum class ThinQueueType : char
{
    Graphics,
    Compute,
    Transfer
};

//! Pipeline stage a shader module belongs to.
enum class ThinShaderStage : uint32_t
{
    Vertex = 1 << 0,
    Hull = 1 << 1,
    Domain = 1 << 2,
    Geometry = 1 << 3,
    Fragment = 1 << 4,
    Compute = 1 << 5,
    RayGeneration = 1 << 6,
    AnyHit = 1 << 7,
    ClosestHit = 1 << 8,
    Miss = 1 << 9,
    Intersection = 1 << 10,
    Callable = 1 << 11
};

inline ThinShaderStage operator|(ThinShaderStage a, ThinShaderStage b) { return (ThinShaderStage)((uint32_t)a | (uint32_t)b); }

inline bool operator&(ThinShaderStage a, ThinShaderStage b) { return ((uint32_t)a & (uint32_t)b) != 0; }

//! What a descriptor binding holds.
enum class ThinDescriptorType : char
{
    ConstantBuffer,
    StorageBuffer,
    SampledTexture,
    StorageTexture,
    Sampler,
    AccelerationStructure
};

//! Element format, shared by vertex attributes and textures.
enum class ThinFormat : uint32_t
{
    Unknown,

    R8Unorm,
    RG8Unorm,
    RGBA8Unorm,
    RGBA8Srgb,
    BGRA8Unorm,

    R16Float,
    RG16Float,
    RGBA16Float,

    R32Float,
    RG32Float,
    RGB32Float,
    RGBA32Float,

    R32Uint,
    R16Uint,

    D32Float,
    D24UnormS8Uint
};

} // namespace bow
