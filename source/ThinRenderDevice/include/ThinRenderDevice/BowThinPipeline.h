#pragma once
#include <ThinRenderDevice/BowThinRenderDevicePredeclares.h>
#include <ThinRenderDevice/ThinRenderDevice_api.h>

namespace bow
{

//! One entry in a descriptor set layout.
struct ThinDescriptorBinding
{
    uint32_t binding = 0;
    ThinDescriptorType type = ThinDescriptorType::ConstantBuffer;
    uint32_t count = 1;
    ThinShaderStage stages = ThinShaderStage::Vertex;
};

//! A set of bindings, which is a descriptor set on one API and a table on the other.
struct ThinDescriptorSetLayoutDescription
{
    std::vector<ThinDescriptorBinding> bindings;
};

//! Constants passed inline rather than through a buffer.
/*!
    Vulkan calls these push constants and DirectX 12 root constants. Both are a
    small block written straight into the command list, and both are limited --
    128 bytes is the figure to design against.
*/
struct ThinRootConstantRange
{
    uint32_t offsetInBytes = 0;
    uint32_t sizeInBytes = 0;
    ThinShaderStage stages = ThinShaderStage::Vertex;
};

//! What a pipeline can be handed: an ordered list of sets plus root constants.
/*!
    This is a Vulkan pipeline layout and a DirectX 12 root signature; the two
    models agree closely enough that one description covers both.
*/
struct ThinPipelineLayoutDescription
{
    std::vector<ThinDescriptorSetLayoutDescription> descriptorSets;
    std::vector<ThinRootConstantRange> rootConstants;
    std::string debugName;
};

class IThinPipelineLayout
{
  public:
    virtual ~IThinPipelineLayout() {}

    virtual const ThinPipelineLayoutDescription &VGetDescription() const = 0;
};

//! One vertex attribute, format separated from the buffer it comes from.
struct ThinVertexAttribute
{
    uint32_t location = 0;
    uint32_t bufferBinding = 0;
    ThinFormat format = ThinFormat::RGB32Float;
    uint32_t offsetInBytes = 0;
    //! Semantic name, needed by DirectX 12 and ignored by Vulkan.
    std::string semanticName = "TEXCOORD";
    uint32_t semanticIndex = 0;
};

//! A vertex buffer slot and the stride it is read with.
struct ThinVertexBufferBinding
{
    uint32_t binding = 0;
    uint32_t strideInBytes = 0;
    bool perInstance = false;
};

enum class ThinPrimitiveTopology : char
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip
};

enum class ThinCompareOp : char
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

enum class ThinCullMode : char
{
    None,
    Front,
    Back
};

enum class ThinBlendFactor : char
{
    Zero,
    One,
    SourceAlpha,
    OneMinusSourceAlpha,
    DestinationAlpha,
    OneMinusDestinationAlpha
};

struct ThinColorAttachmentBlend
{
    bool enabled = false;
    ThinBlendFactor sourceColor = ThinBlendFactor::SourceAlpha;
    ThinBlendFactor destinationColor = ThinBlendFactor::OneMinusSourceAlpha;
    ThinBlendFactor sourceAlpha = ThinBlendFactor::One;
    ThinBlendFactor destinationAlpha = ThinBlendFactor::Zero;
};

//! Everything a graphics pipeline is built from.
/*!
    There are no render pass objects here. Vulkan 1.3 promoted dynamic
    rendering to core and DirectX 12 has always worked that way, so a pipeline
    states the formats it writes to and nothing more.
*/
struct ThinGraphicsPipelineDescription
{
    ThinShaderModulePtr vertexShader;
    ThinShaderModulePtr fragmentShader;
    ThinShaderModulePtr geometryShader;

    std::vector<ThinVertexAttribute> vertexAttributes;
    std::vector<ThinVertexBufferBinding> vertexBuffers;

    ThinPrimitiveTopology topology = ThinPrimitiveTopology::TriangleList;

    ThinCullMode cullMode = ThinCullMode::Back;
    bool frontFaceCounterClockwise = true;
    bool wireframe = false;

    bool depthTestEnabled = false;
    bool depthWriteEnabled = true;
    ThinCompareOp depthCompareOp = ThinCompareOp::Less;

    std::vector<ThinFormat> colorAttachmentFormats;
    std::vector<ThinColorAttachmentBlend> colorAttachmentBlends;
    ThinFormat depthAttachmentFormat = ThinFormat::Unknown;

    ThinPipelineLayoutPtr layout;
    std::string debugName;
};

struct ThinComputePipelineDescription
{
    ThinShaderModulePtr computeShader;
    ThinPipelineLayoutPtr layout;
    std::string debugName;
};

//! Shader groups a ray tracing pipeline dispatches.
struct ThinRayTracingPipelineDescription
{
    ThinShaderModulePtr rayGenerationShader;
    ThinShaderModulePtr missShader;
    ThinShaderModulePtr closestHitShader;
    ThinShaderModulePtr anyHitShader;
    ThinShaderModulePtr intersectionShader;

    uint32_t maxRecursionDepth = 1;

    ThinPipelineLayoutPtr layout;
    std::string debugName;
};

class IThinPipeline
{
  public:
    virtual ~IThinPipeline() {}

    virtual ThinPipelineLayoutPtr VGetLayout() const = 0;
};

//! A filled-in descriptor set, ready to be bound.
class IThinDescriptorSet
{
  public:
    virtual ~IThinDescriptorSet() {}

    virtual void VSetBuffer(uint32_t binding, const ThinBufferPtr &buffer, uint64_t offsetInBytes = 0, uint64_t sizeInBytes = 0) = 0;
    virtual void VSetTexture(uint32_t binding, const ThinTexturePtr &texture) = 0;
    virtual void VSetStorageTexture(uint32_t binding, const ThinTexturePtr &texture) = 0;
    virtual void VSetSampler(uint32_t binding, const ThinSamplerPtr &sampler) = 0;
    virtual void VSetAccelerationStructure(uint32_t binding, const ThinAccelerationStructurePtr &accelerationStructure) = 0;

    //! Pushes the writes made above to the device.
    virtual void VUpdate() = 0;
};

//! Allocates descriptor sets, which is a pool on one API and a heap on the other.
class IThinDescriptorPool
{
  public:
    virtual ~IThinDescriptorPool() {}

    virtual ThinDescriptorSetPtr VAllocate(const ThinDescriptorSetLayoutDescription &layout) = 0;
    virtual void VReset() = 0;
};

} // namespace bow
