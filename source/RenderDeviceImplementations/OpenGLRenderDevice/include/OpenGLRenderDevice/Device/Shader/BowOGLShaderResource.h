#pragma once
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>

#include <OpenGLRenderDevice/Device/Buffer/BowOGLStorageBuffer.h>
#include <OpenGLRenderDevice/Device/Buffer/BowOGLUniformBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>

namespace bow
{

enum class ShaderResourceType : char
{
    Sampler,
    CombinedImageSampler,
    SampledImage,
    SeparateImage,
    StorageImage,

    UniformBuffer,
    StorageBuffer,

    SubpassInput,
    AtomicCounter,
    PushConstant
};

class OGLShaderResource
{
  public:
    OGLShaderResource(uint32_t binding, std::string name, size_t sizeInBytes, ShaderResourceType type) : m_binding(binding), m_name(std::move(name)), m_sizeInBytes(sizeInBytes), m_type(type) {}
    ~OGLShaderResource() {}

    uint32_t GetBinding() const
    {
        FN("OGLShaderResource::GetBinding");

        return m_binding;
    }

    std::string GetName() const
    {
        FN("OGLShaderResource::GetName");

        return m_name;
    }

    size_t GetSizeInBytes() const
    {
        FN("OGLShaderResource::GetSizeInBytes");

        return m_sizeInBytes;
    }

    ShaderResourceType GetResourceType() const
    {
        FN("OGLShaderResource::GetResourceType");

        return m_type;
    }

  private:
    // you shall not copy
    OGLShaderResource(const OGLShaderResource &) = delete;
    OGLShaderResource &operator=(const OGLShaderResource &) = delete;

    uint32_t m_binding;
    std::string m_name;
    size_t m_sizeInBytes;
    ShaderResourceType m_type;
};

} // namespace bow
