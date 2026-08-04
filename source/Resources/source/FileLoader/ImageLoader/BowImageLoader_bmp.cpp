#include "Resources/FileLoader/ImageLoader/BowImageLoader_bmp.h"
#include "Resources/BowResources.h"

#include <iostream>

namespace bow
{

#pragma pack(push, 1)
struct BITMAPFILEHEADER
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BITMAPINFOHEADER
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

ImageLoader_bmp::ImageLoader_bmp() { FN("ImageLoader_bmp::ImageLoader_bmp"); }

ImageLoader_bmp::~ImageLoader_bmp() { FN("ImageLoader_bmp::~ImageLoader_bmp"); }

void ImageLoader_bmp::ImportImage(const char *inputData, Image *outputImage)
{
    FN("ImageLoader_bmp::ImportImage");
    OPTICK_EVENT();

    // Read the BITMAPFILEHEADER
    const BITMAPFILEHEADER fileHeader;
    memcpy((char *)&fileHeader, inputData, sizeof(BITMAPFILEHEADER));

    // Check if it is a valid bitmap file
    if (fileHeader.bfType != 0x4D42)
    { // 'BM' in little-endian
        LOG_ERROR("Not a valid bitmap file!");
        return;
    }

    // Read the BITMAPINFOHEADER
    const BITMAPINFOHEADER infoHeader;
    memcpy((char *)&infoHeader, inputData + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

    // Log the file header information
    LOG_TRACE("Bitmap File Header:");
    LOG_TRACE("  bfType: %d", fileHeader.bfType);
    LOG_TRACE("  bfSize: %d Bytes (Size of the bitmap file)", fileHeader.bfSize);
    LOG_TRACE("  bfReserved1: %d (Reserved, should be 0)", fileHeader.bfReserved1);
    LOG_TRACE("  bfReserved2: %d (Reserved, should be 0)", fileHeader.bfReserved2);
    LOG_TRACE("  bfOffBits: %d (Offset to start of bitmap data)", fileHeader.bfOffBits);

    // Log the info header information
    LOG_TRACE("Bitmap Info Header:");
    LOG_TRACE("  biSize: %d Bytes (Size of this header, typical for BITMAPINFOHEADER)", infoHeader.biSize);
    LOG_TRACE("  biWidth: %d Pixels (Width of the image)", infoHeader.biWidth);
    LOG_TRACE("  biHeight: %d Pixels (Height of the image)", infoHeader.biHeight);
    LOG_TRACE("  biPlanes: %d (Number of color planes, should always be 1)", infoHeader.biPlanes);
    LOG_TRACE("  biBitCount: %d Bits (Color depth, 24 bits means 8 bits per channel, RGB)", infoHeader.biBitCount);
    LOG_TRACE("  biCompression: %d (Compression type)", infoHeader.biCompression);
    LOG_TRACE("  biSizeImage: %d Bytes (Size of the image data, excluding headers)", infoHeader.biSizeImage);
    LOG_TRACE("  biXPelsPerMeter: %d Pixels per meter (Horizontal resolution)", infoHeader.biXPelsPerMeter);
    LOG_TRACE("  biYPelsPerMeter: %d Pixels per meter (Vertical resolution)", infoHeader.biYPelsPerMeter);
    LOG_TRACE("  biClrUsed: %d (Number of colors actually used, 0 means all)", infoHeader.biClrUsed);
    LOG_TRACE("  biClrImportant: %d (Number of important colors, 0 means all)", infoHeader.biClrImportant);

    // Set the output image properties
    outputImage->m_width = infoHeader.biWidth;
    outputImage->m_height = infoHeader.biHeight;
    outputImage->m_numBitsPerPixel = 32;
    outputImage->m_numberOfChannels = 4;
    outputImage->m_format = Image::Format::uchar;
    outputImage->m_sizeInBytes = infoHeader.biWidth * infoHeader.biHeight * outputImage->m_numberOfChannels;
    outputImage->m_data.resize(outputImage->m_sizeInBytes);

    // Read the bitmap data and convert to RGBA
    const unsigned char *rgb_data = reinterpret_cast<const unsigned char *>(inputData + fileHeader.bfOffBits);
    for (int y = 0; y < outputImage->m_height; ++y)
    {
        for (int x = 0; x < outputImage->m_width; ++x)
        {
            size_t rgbIndex = (y * outputImage->m_width + x) * 3;  // RGB has 3 channels
            size_t rgbaIndex = (y * outputImage->m_width + x) * 4; // RGBA has 4 channels

            outputImage->m_data[rgbaIndex] = rgb_data[rgbIndex + 2];     // B
            outputImage->m_data[rgbaIndex + 1] = rgb_data[rgbIndex + 1]; // G
            outputImage->m_data[rgbaIndex + 2] = rgb_data[rgbIndex];     // R
            if (infoHeader.biBitCount == 24)
            {
                outputImage->m_data[rgbaIndex + 3] = 255; // A, full opacity
            }
            else if (infoHeader.biBitCount == 32)
            {
                outputImage->m_data[rgbaIndex + 3] = rgb_data[rgbIndex + 3]; // A
            }
        }
    }
}

} // namespace bow