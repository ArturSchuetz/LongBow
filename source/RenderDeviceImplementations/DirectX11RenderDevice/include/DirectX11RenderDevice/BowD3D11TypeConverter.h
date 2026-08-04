#pragma once
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/Context/VertexAttributeBindings/BowComponentDatatype.h>
#include <RenderDevice/Device/IBowRenderContext.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>
#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

#include <d3d11.h>

namespace bow
{

//! Translates the engine enums into their DirectX 11 equivalents.
class D3D11TypeConverter
{
  public:
    static DXGI_FORMAT ToDXGIFormat(TextureFormat format);
    static DXGI_FORMAT ToDXGIFormat(ComponentDatatype datatype, int componentCount);
    static D3D11_PRIMITIVE_TOPOLOGY ToPrimitiveTopology(PrimitiveType primitiveType);
    static D3D11_FILTER ToFilter(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter);
    static D3D11_TEXTURE_ADDRESS_MODE ToAddressMode(TextureWrap wrap);

    //! Size of one pixel, for working out an upload row pitch.
    static uint32_t BytesPerPixel(ImageFormat format, ImageDatatype dataType);
};

} // namespace bow
