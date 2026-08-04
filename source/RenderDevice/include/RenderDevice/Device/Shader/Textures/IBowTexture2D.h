#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

enum class TextureFormat : char
{
    Undefined,
    RedGreenBlue8,
    RedGreenBlue16,
    RedGreenBlueAlpha8,
    RedGreenBlue10A2,
    RedGreenBlueAlpha16,
    Depth16,
    Depth24,
    Red8,
    Red16,
    RedGreen8,
    RedGreen16,
    Red16f,
    Red32f,
    RedGreen16f,
    RedGreen32f,
    Red8i,
    Red8ui,
    Red16i,
    Red16ui,
    Red32i,
    Red32ui,
    RedGreen8i,
    RedGreen8ui,
    RedGreen16i,
    RedGreen16ui,
    RedGreen32i,
    RedGreen32ui,
    RedGreenBlueAlpha32f,
    RedGreenBlue32f,
    RedGreenBlueAlpha16f,
    RedGreenBlue16f,
    Depth24Stencil8,
    Red11fGreen11fBlue10f,
    RedGreenBlue9E5,
    SRedGreenBlue8,
    SRedGreenBlue8Alpha8,
    Depth32f,
    Depth32fStencil8,
    RedGreenBlueAlpha32ui,
    RedGreenBlue32ui,
    RedGreenBlueAlpha16ui,
    RedGreenBlue16ui,
    RedGreenBlueAlpha8ui,
    RedGreenBlue8ui,
    RedGreenBlueAlpha32i,
    RedGreenBlue32i,
    RedGreenBlueAlpha16i,
    RedGreenBlue16i,
    RedGreenBlueAlpha8i,
    RedGreenBlue8i,
    // Additional formats
    RedGreenBlue4Alpha4,      // VK_FORMAT_R4G4B4A4_UNORM_PACK16
    BlueGreenRed4Alpha4,      // VK_FORMAT_B4G4R4A4_UNORM_PACK16
    Red5Green6Blue5,          // VK_FORMAT_R5G6B5_UNORM_PACK16
    Blue5Green6Red5,          // VK_FORMAT_B5G6R5_UNORM_PACK16
    Red5Green5Blue5Alpha1,    // VK_FORMAT_R5G5B5A1_UNORM_PACK16
    Blue5Green5Red5Alpha1,    // VK_FORMAT_B5G5R5A1_UNORM_PACK16
    Alpha1Red5Green5Blue5,    // VK_FORMAT_A1R5G5B5_UNORM_PACK16
    Alpha2Red10Green10Blue10, // VK_FORMAT_A2R10G10B10_UNORM_PACK32
    Alpha2Blue10Green10Red10, // VK_FORMAT_A2B10G10R10_UNORM_PACK32
    Blue8Green8Red8,          // VK_FORMAT_B8G8R8_UNORM
    Blue8Green8Red8Alpha8,    // VK_FORMAT_B8G8R8A8_UNORM
    Alpha8Blue8Green8Red8,    // VK_FORMAT_A8B8G8R8_UNORM_PACK32
    Blue10Green11Red11,       // VK_FORMAT_B10G11R11_UFLOAT_PACK32
    Blue9Green9Red9E5         // VK_FORMAT_E5B9G9R9_UFLOAT_PACK32
};

struct Texture2DDescription
{
  public:
    Texture2DDescription(int width, int height, TextureFormat format, bool generateMipmaps = false) : m_width(width), m_height(height), m_format(format), m_generateMipmaps(generateMipmaps) {}

    uint32_t GetWidth() const { return m_width; }

    uint32_t GetHeight() const { return m_height; }

    TextureFormat GetTextureFormat() const { return m_format; }

    bool GenerateMipmaps() const { return m_generateMipmaps; }

    bool ColorRenderable() const { return !DepthRenderable() && !DepthStencilRenderable(); }

    bool DepthRenderable() const { return m_format == TextureFormat::Depth16 || m_format == TextureFormat::Depth24 || m_format == TextureFormat::Depth32f || m_format == TextureFormat::Depth24Stencil8 || m_format == TextureFormat::Depth32fStencil8; }

    bool DepthStencilRenderable() const { return m_format == TextureFormat::Depth24Stencil8 || m_format == TextureFormat::Depth32fStencil8; }

    int GetApproximateSizeInBytes() const { return m_width * m_height * GetSizeInBytes(m_format); }

    bool operator==(const Texture2DDescription &right) const { return (m_width == right.m_width) && (m_height == right.m_height) && (m_format == right.m_format) && (m_generateMipmaps == right.m_generateMipmaps); }

    bool operator!=(const Texture2DDescription &right) const { return (*this == right); }

  private:
    uint32_t GetSizeInBytes(TextureFormat format) const
    {
        switch (format)
        {
        case TextureFormat::RedGreenBlue8:
            return 3;
        case TextureFormat::RedGreenBlue16:
            return 6;
        case TextureFormat::RedGreenBlueAlpha8:
            return 4;
        case TextureFormat::RedGreenBlue10A2:
            return 4;
        case TextureFormat::RedGreenBlueAlpha16:
            return 8;
        case TextureFormat::Depth16:
            return 2;
        case TextureFormat::Depth24:
            return 3;
        case TextureFormat::Red8:
            return 1;
        case TextureFormat::Red16:
            return 2;
        case TextureFormat::RedGreen8:
            return 2;
        case TextureFormat::RedGreen16:
            return 4;
        case TextureFormat::Red16f:
            return 2;
        case TextureFormat::Red32f:
            return 4;
        case TextureFormat::RedGreen16f:
            return 4;
        case TextureFormat::RedGreen32f:
            return 8;
        case TextureFormat::Red8i:
            return 1;
        case TextureFormat::Red8ui:
            return 1;
        case TextureFormat::Red16i:
            return 2;
        case TextureFormat::Red16ui:
            return 2;
        case TextureFormat::Red32i:
            return 4;
        case TextureFormat::Red32ui:
            return 4;
        case TextureFormat::RedGreen8i:
            return 2;
        case TextureFormat::RedGreen8ui:
            return 2;
        case TextureFormat::RedGreen16i:
            return 4;
        case TextureFormat::RedGreen16ui:
            return 4;
        case TextureFormat::RedGreen32i:
            return 8;
        case TextureFormat::RedGreen32ui:
            return 8;
        case TextureFormat::RedGreenBlueAlpha32f:
            return 16;
        case TextureFormat::RedGreenBlue32f:
            return 12;
        case TextureFormat::RedGreenBlueAlpha16f:
            return 8;
        case TextureFormat::RedGreenBlue16f:
            return 6;
        case TextureFormat::Depth24Stencil8:
            return 4;
        case TextureFormat::Red11fGreen11fBlue10f:
            return 4;
        case TextureFormat::RedGreenBlue9E5:
            return 4;
        case TextureFormat::SRedGreenBlue8:
            return 3;
        case TextureFormat::SRedGreenBlue8Alpha8:
            return 4;
        case TextureFormat::Depth32f:
            return 4;
        case TextureFormat::Depth32fStencil8:
            return 5;
        case TextureFormat::RedGreenBlueAlpha32ui:
            return 16;
        case TextureFormat::RedGreenBlue32ui:
            return 12;
        case TextureFormat::RedGreenBlueAlpha16ui:
            return 8;
        case TextureFormat::RedGreenBlue16ui:
            return 6;
        case TextureFormat::RedGreenBlueAlpha8ui:
            return 4;
        case TextureFormat::RedGreenBlue8ui:
            return 3;
        case TextureFormat::RedGreenBlueAlpha32i:
            return 16;
        case TextureFormat::RedGreenBlue32i:
            return 12;
        case TextureFormat::RedGreenBlueAlpha16i:
            return 8;
        case TextureFormat::RedGreenBlue16i:
            return 6;
        case TextureFormat::RedGreenBlueAlpha8i:
            return 4;
        case TextureFormat::RedGreenBlue8i:
            return 3;
        }
        return 0;
    }

    const uint32_t m_width;
    const uint32_t m_height;
    const TextureFormat m_format;
    const bool m_generateMipmaps;
};

enum class ImageDatatype : char
{
    Byte,
    UnsignedByte,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Float,
    HalfFloat,
    UnsignedByte332,
    UnsignedShort4444,
    UnsignedShort5551,
    UnsignedInt8888,
    UnsignedInt1010102,
    UnsignedByte233Reversed,
    UnsignedShort565,
    UnsignedShort565Reversed,
    UnsignedShort4444Reversed,
    UnsignedShort1555Reversed,
    UnsignedInt8888Reversed,
    UnsignedInt2101010Reversed,
    UnsignedInt248,
    UnsignedInt10F11F11FReversed,
    UnsignedInt5999Reversed,
    Float32UnsignedInt248Reversed
};

enum class ImageFormat : char
{
    StencilIndex,
    DepthComponent,
    Red,
    Green,
    Blue,
    RedGreenBlue,
    RedGreenBlueAlpha,
    BlueGreenRed,
    BlueGreenRedAlpha,
    RedGreen,
    RedGreenInteger,
    DepthStencil,
    RedInteger,
    GreenInteger,
    BlueInteger,
    RedGreenBlueInteger,
    RedGreenBlueAlphaInteger,
    BlueGreenRedInteger,
    BlueGreenRedAlphaInteger
};

class ITexture2D
{
  public:
    virtual ~ITexture2D() {}

    virtual void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4)
    {
        VCopyFromBuffer(pixelBuffer, 0, 0, VGetDescription().GetWidth(), VGetDescription().GetHeight(), format, dataType, rowAlignment = 4);
    }

    virtual void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) = 0;

    virtual void VCopyFromSystemMemory(void *bitmapInSystemMemory, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4)
    {
        VCopyFromSystemMemory(bitmapInSystemMemory, VGetDescription().GetWidth(), VGetDescription().GetHeight(), format, dataType, rowAlignment = 4);
    }

    virtual void VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) = 0;

    virtual std::shared_ptr<void> VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) = 0;

    virtual Texture2DDescription VGetDescription() = 0;
};

inline uint32_t NumberOfChannels(ImageFormat format)
{
    switch (format)
    {
    case ImageFormat::StencilIndex:
        return 1;
    case ImageFormat::DepthComponent:
        return 1;
    case ImageFormat::Red:
        return 1;
    case ImageFormat::Green:
        return 1;
    case ImageFormat::Blue:
        return 1;
    case ImageFormat::RedGreenBlue:
        return 3;
    case ImageFormat::RedGreenBlueAlpha:
        return 4;
    case ImageFormat::BlueGreenRed:
        return 3;
    case ImageFormat::BlueGreenRedAlpha:
        return 4;
    case ImageFormat::RedGreen:
        return 2;
    case ImageFormat::RedGreenInteger:
        return 2;
    case ImageFormat::DepthStencil:
        return 2;
    case ImageFormat::RedInteger:
        return 1;
    case ImageFormat::GreenInteger:
        return 1;
    case ImageFormat::BlueInteger:
        return 1;
    case ImageFormat::RedGreenBlueInteger:
        return 3;
    case ImageFormat::RedGreenBlueAlphaInteger:
        return 4;
    case ImageFormat::BlueGreenRedInteger:
        return 3;
    case ImageFormat::BlueGreenRedAlphaInteger:
        return 4;
    }

    LOG_FATAL("ImageFormat does not exist.");
    return -1;
}

inline uint32_t SizeInBytes(TextureFormat dataType)
{
    switch (dataType)
    {
    case TextureFormat::RedGreenBlue8:
        return (8 * 3) / 8;
    case TextureFormat::RedGreenBlue16:
        return (16 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha8:
        return (8 * 4) / 8;
    case TextureFormat::RedGreenBlue10A2:
        return ((10 * 3) + 2) / 8;
    case TextureFormat::RedGreenBlueAlpha16:
        return (16 * 4) / 8;
    case TextureFormat::Depth16:
        return (16) / 8;
    case TextureFormat::Depth24:
        return (24) / 8;
    case TextureFormat::Red8:
        return (8) / 8;
    case TextureFormat::Red16:
        return (16) / 8;
    case TextureFormat::RedGreen8:
        return (8 * 2) / 8;
    case TextureFormat::RedGreen16:
        return (16 * 2) / 8;
    case TextureFormat::Red16f:
        return (16) / 8;
    case TextureFormat::Red32f:
        return (32) / 8;
    case TextureFormat::RedGreen16f:
        return (16 * 2) / 8;
    case TextureFormat::RedGreen32f:
        return (32 * 2) / 8;
    case TextureFormat::Red8i:
        return (8 * 1) / 8;
    case TextureFormat::Red8ui:
        return (8 * 1) / 8;
    case TextureFormat::Red16i:
        return (16 * 1) / 8;
    case TextureFormat::Red16ui:
        return (16 * 1) / 8;
    case TextureFormat::Red32i:
        return (32 * 1) / 8;
    case TextureFormat::Red32ui:
        return (32 * 1) / 8;
    case TextureFormat::RedGreen8i:
        return (8 * 2) / 8;
    case TextureFormat::RedGreen8ui:
        return (8 * 2) / 8;
    case TextureFormat::RedGreen16i:
        return (16 * 2) / 8;
    case TextureFormat::RedGreen16ui:
        return (16 * 2) / 8;
    case TextureFormat::RedGreen32i:
        return (32 * 2) / 8;
    case TextureFormat::RedGreen32ui:
        return (32 * 2) / 8;
    case TextureFormat::RedGreenBlueAlpha32f:
        return (32 * 4) / 8;
    case TextureFormat::RedGreenBlue32f:
        return (32 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha16f:
        return (16 * 4) / 8;
    case TextureFormat::RedGreenBlue16f:
        return (16 * 3) / 8;
    case TextureFormat::Red11fGreen11fBlue10f:
        return (11 + 11 + 10) / 8;
    case TextureFormat::RedGreenBlue9E5:
        return ((9 * 3) + 5) / 8;
    case TextureFormat::SRedGreenBlue8:
        return (8 * 3) / 8;
    case TextureFormat::SRedGreenBlue8Alpha8:
        return (8 * 4) / 8;
    case TextureFormat::Depth32f:
        return (32 * 1) / 8;
    case TextureFormat::Depth32fStencil8:
        return (32 + 8) / 8;
    case TextureFormat::RedGreenBlueAlpha32ui:
        return (32 * 4) / 8;
    case TextureFormat::RedGreenBlue32ui:
        return (32 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha16ui:
        return (16 * 4) / 8;
    case TextureFormat::RedGreenBlue16ui:
        return (16 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha8ui:
        return (8 * 4) / 8;
    case TextureFormat::RedGreenBlue8ui:
        return (8 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha32i:
        return (32 * 4) / 8;
    case TextureFormat::RedGreenBlue32i:
        return (32 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha16i:
        return (16 * 4) / 8;
    case TextureFormat::RedGreenBlue16i:
        return (16 * 3) / 8;
    case TextureFormat::RedGreenBlueAlpha8i:
        return (8 * 4) / 8;
    case TextureFormat::RedGreenBlue8i:
        return (8 * 3) / 8;
    }

    LOG_FATAL("TextureFormat does not exist.");
    return -1;
}

inline int SizeInBytes(ImageDatatype dataType)
{
    switch (dataType)
    {
    case ImageDatatype::Byte:
        return 1;
    case ImageDatatype::UnsignedByte:
        return 1;
    case ImageDatatype::Short:
        return 2;
    case ImageDatatype::UnsignedShort:
        return 2;
    case ImageDatatype::Int:
        return 4;
    case ImageDatatype::UnsignedInt:
        return 4;
    case ImageDatatype::Float:
        return 4;
    case ImageDatatype::HalfFloat:
        return 2;
    case ImageDatatype::UnsignedByte332:
        return 1;
    case ImageDatatype::UnsignedShort4444:
        return 2;
    case ImageDatatype::UnsignedShort5551:
        return 2;
    case ImageDatatype::UnsignedInt8888:
        return 4;
    case ImageDatatype::UnsignedInt1010102:
        return 4;
    case ImageDatatype::UnsignedByte233Reversed:
        return 1;
    case ImageDatatype::UnsignedShort565:
        return 2;
    case ImageDatatype::UnsignedShort565Reversed:
        return 2;
    case ImageDatatype::UnsignedShort4444Reversed:
        return 2;
    case ImageDatatype::UnsignedShort1555Reversed:
        return 2;
    case ImageDatatype::UnsignedInt8888Reversed:
        return 4;
    case ImageDatatype::UnsignedInt2101010Reversed:
        return 4;
    case ImageDatatype::UnsignedInt248:
        return 4;
    case ImageDatatype::UnsignedInt10F11F11FReversed:
        return 4;
    case ImageDatatype::UnsignedInt5999Reversed:
        return 4;
    case ImageDatatype::Float32UnsignedInt248Reversed:
        return 4;
    }

    LOG_FATAL("ImageDatatype does not exist.");
    return -1;
}

inline void VerifyRowAlignment(int rowAlignment)
{
    if ((rowAlignment != 1) && (rowAlignment != 2) && (rowAlignment != 4) && (rowAlignment != 8))
    {
        LOG_FATAL("rowAlignment does not exist.");
    }
}

inline bool IsPowerOfTwo(uint32_t i) { return (i != 0) && ((i & (i - 1)) == 0); }

inline int RequiredSizeInBytes(int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    int rowSize = width * NumberOfChannels(format) * SizeInBytes(dataType);

    int remainder = (rowSize % rowAlignment);
    rowSize += (rowAlignment - remainder) % rowAlignment;

    return rowSize * height;
}

} // namespace bow
