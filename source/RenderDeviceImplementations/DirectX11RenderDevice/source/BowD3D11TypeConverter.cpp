#include <DirectX11RenderDevice/BowD3D11TypeConverter.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

DXGI_FORMAT D3D11TypeConverter::ToDXGIFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::RedGreenBlue8:
        // DirectX has no three-channel 8-bit format; the alpha is padding.
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::RedGreenBlueAlpha8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::RedGreenBlue16f:
    case TextureFormat::RedGreenBlueAlpha16f:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case TextureFormat::RedGreenBlue32f:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case TextureFormat::RedGreenBlueAlpha32f:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case TextureFormat::Depth24Stencil8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
}

DXGI_FORMAT D3D11TypeConverter::ToDXGIFormat(ComponentDatatype datatype, int componentCount)
{
    if (datatype == ComponentDatatype::Float)
    {
        switch (componentCount)
        {
        case 1:
            return DXGI_FORMAT_R32_FLOAT;
        case 2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case 3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        default:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    if (datatype == ComponentDatatype::UnsignedByte)
    {
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    LOG_ERROR("Unsupported vertex component datatype.");
    return DXGI_FORMAT_R32G32B32A32_FLOAT;
}

D3D11_PRIMITIVE_TOPOLOGY D3D11TypeConverter::ToPrimitiveTopology(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
    case PrimitiveType::Points:
        return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case PrimitiveType::Lines:
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveType::LineStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case PrimitiveType::Triangles:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveType::TriangleStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    default:
        // Line loops and triangle fans have no DirectX equivalent; they were
        // dropped from the API rather than renamed.
        LOG_ERROR("Primitive type has no DirectX 11 equivalent, drawing as a triangle list.");
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}

D3D11_FILTER D3D11TypeConverter::ToFilter(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter)
{
    const bool magLinear = (magnificationFilter == TextureMagnificationFilter::Linear);
    const bool minLinear = (minificationFilter == TextureMinificationFilter::Linear || minificationFilter == TextureMinificationFilter::LinearMipmapLinear || minificationFilter == TextureMinificationFilter::LinearMipmapNearest);
    const bool mipLinear = (minificationFilter == TextureMinificationFilter::LinearMipmapLinear || minificationFilter == TextureMinificationFilter::NearestMipmapLinear);

    if (minLinear && magLinear && mipLinear)
    {
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }
    if (minLinear && magLinear)
    {
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    }
    if (magLinear)
    {
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    }
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

D3D11_TEXTURE_ADDRESS_MODE D3D11TypeConverter::ToAddressMode(TextureWrap wrap)
{
    switch (wrap)
    {
    case TextureWrap::Repeat:
        return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureWrap::MirroredRepeat:
        return D3D11_TEXTURE_ADDRESS_MIRROR;
    case TextureWrap::Clamp:
    default:
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    }
}

uint32_t D3D11TypeConverter::BytesPerPixel(ImageFormat format, ImageDatatype dataType)
{
    uint32_t channels = 0;
    switch (format)
    {
    case ImageFormat::Red:
    case ImageFormat::Green:
    case ImageFormat::Blue:
    case ImageFormat::DepthComponent:
        channels = 1;
        break;
    case ImageFormat::RedGreenBlue:
    case ImageFormat::BlueGreenRed:
        channels = 3;
        break;
    case ImageFormat::RedGreenBlueAlpha:
    case ImageFormat::BlueGreenRedAlpha:
        channels = 4;
        break;
    default:
        return 0;
    }

    uint32_t bytes = 0;
    switch (dataType)
    {
    case ImageDatatype::Byte:
    case ImageDatatype::UnsignedByte:
        bytes = 1;
        break;
    case ImageDatatype::Short:
    case ImageDatatype::UnsignedShort:
    case ImageDatatype::HalfFloat:
        bytes = 2;
        break;
    case ImageDatatype::Int:
    case ImageDatatype::UnsignedInt:
    case ImageDatatype::Float:
        bytes = 4;
        break;
    default:
        return 0;
    }

    return channels * bytes;
}

} // namespace bow
