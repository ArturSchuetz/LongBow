#include <DirectX11RenderDevice/Device/Textures/BowD3D11Texture2D.h>

#include <DirectX11RenderDevice/BowD3D11TypeConverter.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

D3D11Texture2D::D3D11Texture2D(ID3D11Device *device, ID3D11DeviceContext *context, Texture2DDescription description) : m_device(device), m_context(context), m_description(description)
{
    FN("D3D11Texture2D::D3D11Texture2D");

    LOG_ASSERT(!(description.GetWidth() == 0), "description.Width must be greater than zero.");
    LOG_ASSERT(!(description.GetHeight() == 0), "description.Height must be greater than zero.");

    D3D11_TEXTURE2D_DESC textureDescription = {};
    textureDescription.Width = description.GetWidth();
    textureDescription.Height = description.GetHeight();
    // Zero levels means "as many as the size allows", which the driver fills
    // in when GenerateMips is called.
    textureDescription.MipLevels = description.GenerateMipmaps() ? 0 : 1;
    textureDescription.ArraySize = 1;
    textureDescription.Format = D3D11TypeConverter::ToDXGIFormat(description.GetTextureFormat());
    textureDescription.SampleDesc.Count = 1;
    textureDescription.Usage = D3D11_USAGE_DEFAULT;
    textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if (description.GenerateMipmaps())
    {
        // Generating mips is a render-target operation on this API, so the
        // texture has to be created as one even though nothing draws into it.
        textureDescription.BindFlags |= D3D11_BIND_RENDER_TARGET;
        textureDescription.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }

    HRESULT result = device->CreateTexture2D(&textureDescription, nullptr, &m_texture);
    if (FAILED(result))
    {
        LOG_ERROR("CreateTexture2D failed (0x%08X).", (unsigned)result);
        return;
    }

    result = device->CreateShaderResourceView(m_texture.Get(), nullptr, &m_shaderResourceView);
    if (FAILED(result))
    {
        LOG_ERROR("CreateShaderResourceView failed (0x%08X).", (unsigned)result);
    }
}

D3D11Texture2D::~D3D11Texture2D() { FN("D3D11Texture2D::~D3D11Texture2D"); }

void D3D11Texture2D::VCopyFromSystemMemory(void *bitmapInSystemMemory, ImageFormat format, ImageDatatype dataType, int rowAlignment)
{
    VCopyFromSystemMemory(bitmapInSystemMemory, (int)m_description.GetWidth(), (int)m_description.GetHeight(), format, dataType, rowAlignment);
}

void D3D11Texture2D::VCopyFromSystemMemory(void *bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int /*rowAlignment*/)
{
    FN("D3D11Texture2D::VCopyFromSystemMemory");

    if (m_texture == nullptr || bitmapInSystemMemory == nullptr)
    {
        return;
    }

    const uint32_t bytesPerPixel = D3D11TypeConverter::BytesPerPixel(format, dataType);
    if (bytesPerPixel == 0)
    {
        LOG_ERROR("Unsupported image format for a texture upload.");
        return;
    }

    D3D11_BOX box = {};
    box.right = (UINT)width;
    box.bottom = (UINT)height;
    box.back = 1;

    m_context->UpdateSubresource(m_texture.Get(), 0, &box, bitmapInSystemMemory, (UINT)(width * bytesPerPixel), 0);

    if (m_description.GenerateMipmaps() && m_shaderResourceView != nullptr)
    {
        m_context->GenerateMips(m_shaderResourceView.Get());
    }
}

void D3D11Texture2D::VCopyFromBuffer(WritePixelBufferPtr, ImageFormat, ImageDatatype, int)
{
    FN("D3D11Texture2D::VCopyFromBuffer");

    LOG_ERROR("DirectX 11: pixel buffers are not implemented yet.");
}

void D3D11Texture2D::VCopyFromBuffer(WritePixelBufferPtr, int, int, int, int, ImageFormat, ImageDatatype, int)
{
    FN("D3D11Texture2D::VCopyFromBuffer");

    LOG_ERROR("DirectX 11: pixel buffers are not implemented yet.");
}

std::shared_ptr<void> D3D11Texture2D::VCopyToSystemMemory(ImageFormat /*format*/, ImageDatatype /*dataType*/, int /*rowAlignment*/)
{
    FN("D3D11Texture2D::VCopyToSystemMemory");

    LOG_ERROR("DirectX 11: texture read-back is not implemented yet.");
    return nullptr;
}

Texture2DDescription D3D11Texture2D::VGetDescription()
{
    FN("D3D11Texture2D::VGetDescription");

    return m_description;
}

} // namespace bow
