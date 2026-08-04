#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <CoreSystems/BowCorePredeclares.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

namespace spirv_cross
{
class SPIRType;
}

namespace bow
{

class VulkanTypeConverter
{
  public:
    static std::string ToString(VkResult result);
    static std::string ToString(VkFormat format);

    static VkFormat ToVkFormat(TextureFormat format);
    static TextureFormat ToTextureFormat(VkFormat);
    static ImageFormat TextureToImageFormat(TextureFormat format);
    static VkFormat ToVkFormat(ShaderVertexAttributeType type);
    static VkFilter ToVkFilter(TextureMagnificationFilter filter);
    static VkFilter ToVkFilter(TextureMinificationFilter filter);
    static VkSamplerAddressMode ToVkSamplerAddressMode(TextureWrap wrap);

    static VkBufferUsageFlags VulkanTypeConverter::ToVkBufferUsageFlags(BufferHint hint);
    static VkIndexType ToVkIndexType(IndexBufferDatatype dataType);
    static ShaderVertexAttributeType ToShaderVertexAttributeType(const spirv_cross::SPIRType &type);
    static VkBool32 ToVkBool32(bool boolean);
    static VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveType primitiveType);
    static VkBlendFactor ToVkBlendFactor(SourceBlendingFactor factor);
    static VkBlendFactor ToVkBlendFactor(DestinationBlendingFactor factor);
    static VkBlendOp ToVkBlendOp(BlendEquation equation);
    static VkCompareOp ToVkCompareOp(DepthTestFunction function);
    static VkCompareOp ToVkCompareOp(StencilTestFunction function);
    static VkStencilOp ToVkStencilOp(StencilOperation operation);
    static VkCullModeFlags ToVkCullMode(CullFace cullFace);
    static VkFrontFace ToVkFrontFace(WindingOrder windingOrder);
    static VkPolygonMode ToVkPolygonMode(RasterizationMode mode);

  private:
    VulkanTypeConverter() = delete;
    VulkanTypeConverter(const VulkanTypeConverter &) = delete;
    VulkanTypeConverter &operator=(const VulkanTypeConverter &) = delete;
};

} // namespace bow
