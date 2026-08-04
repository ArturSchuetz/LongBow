#pragma once
#include <ThinRenderDevice/BowThinRenderDevicePredeclares.h>
#include <ThinRenderDevice/ThinRenderDevice_api.h>

namespace bow
{

//! How a buffer will be used and where it should live.
struct ThinBufferDescription
{
    uint64_t sizeInBytes = 0;

    bool vertexBuffer = false;
    bool indexBuffer = false;
    bool constantBuffer = false;
    bool storageBuffer = false;
    bool indirectArgument = false;
    bool accelerationStructureInput = false;

    //! Visible to the CPU, for data written every frame.
    /*!
        Vulkan calls this host-visible memory, DirectX 12 an upload heap. Both
        mean the same trade: writable without a copy, slower for the GPU to
        read. A buffer that is filled once should leave this false and be
        uploaded through a staging copy instead.
    */
    bool cpuVisible = false;

    //! Read back by the CPU after the GPU has written it.
    bool cpuReadable = false;

    std::string debugName;
};

//! How a texture will be used.
struct ThinTextureDescription
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    uint32_t sampleCount = 1;

    ThinFormat format = ThinFormat::RGBA8Unorm;

    bool sampled = false;
    bool storage = false;
    bool renderTarget = false;
    bool depthStencil = false;
    bool copySource = false;
    bool copyDestination = false;

    std::string debugName;
};

//! Filtering and addressing.
struct ThinSamplerDescription
{
    bool magLinear = true;
    bool minLinear = true;
    bool mipLinear = true;

    enum class AddressMode : char
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    AddressMode addressU = AddressMode::Repeat;
    AddressMode addressV = AddressMode::Repeat;
    AddressMode addressW = AddressMode::Repeat;

    float maxAnisotropy = 1.0f;
    float minLod = 0.0f;
    float maxLod = 1000.0f;
};

//! A buffer on the device.
class IThinBuffer
{
  public:
    virtual ~IThinBuffer() {}

    virtual const ThinBufferDescription &VGetDescription() const = 0;

    //! Maps a CPU-visible buffer, or returns null when it is device-local.
    virtual void *VMap() = 0;
    virtual void VUnmap() = 0;

    //! Address the shader uses to reach this buffer, where the API exposes one.
    virtual uint64_t VGetDeviceAddress() const = 0;
};

//! A texture on the device.
class IThinTexture
{
  public:
    virtual ~IThinTexture() {}

    virtual const ThinTextureDescription &VGetDescription() const = 0;
};

//! Filtering state, as an object both APIs want created up front.
class IThinSampler
{
  public:
    virtual ~IThinSampler() {}
};

//! Compiled shader bytecode.
/*!
    Both APIs consume compiled bytecode rather than source: SPIR-V for Vulkan,
    DXIL for DirectX 12. `dxc` produces either from one HLSL source, which is
    why the thin tier is authored in HLSL.
*/
class IThinShaderModule
{
  public:
    virtual ~IThinShaderModule() {}

    virtual ThinShaderStage VGetStage() const = 0;
};

} // namespace bow
