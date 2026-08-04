#include "Resources/FileLoader/ImageLoader/BowImageLoader_png.h"
#include "Resources/BowResources.h"

#include "CoreSystems/BowLogger.h"

#include "LoadPNG/lodepng.h"

namespace bow
{

ImageLoader_png::ImageLoader_png() { FN("ImageLoader_png::ImageLoader_png"); }

ImageLoader_png::~ImageLoader_png() { FN("ImageLoader_png::~ImageLoader_png"); }

void ImageLoader_png::ImportImage(const std::vector<uint8_t> &inputData, Image *outputImage)
{
    FN("ImageLoader_png::ImportImage");
    OPTICK_EVENT();

    lodepng::State state; // optionally customize this one

    uint32_t width, height;
    std::vector<uint8_t> image_data;
    unsigned int error = lodepng::decode(image_data, width, height, state, inputData);
    if (!error)
    {
        uint32_t channels = -1;
        switch (state.info_png.color.colortype)
        {
        case LCT_GREY:
            channels = 1;
            break;
        case LCT_RGB:
            channels = 3;
            break;
        case LCT_PALETTE:
            channels = 1;
            break;
        case LCT_GREY_ALPHA:
            channels = 2;
            break;
        case LCT_RGBA:
            channels = 4;
            break;
        default:
            LOG_FATAL("ImageLoader_png: Unsupported color type: %d", state.info_png.color.colortype);
        }

        outputImage->m_width = width;
        outputImage->m_height = height;
        outputImage->m_numBitsPerPixel = 32;
        outputImage->m_numberOfChannels = 4;
        outputImage->m_format = Image::Format::uchar;
        outputImage->m_sizeInBytes = outputImage->m_numberOfChannels * outputImage->m_width * outputImage->m_height;
        outputImage->m_data.resize(outputImage->m_sizeInBytes);

        if (channels == 1)
        {
            for (int y = 0; y < outputImage->m_height; ++y)
            {
                for (int x = 0; x < outputImage->m_width; ++x)
                {
                    size_t rgbIndex = (y * outputImage->m_width + x) * 4;  // RGB has 3 channels
                    size_t rgbaIndex = (y * outputImage->m_width + x) * 4; // RGBA has 4 channels

                    outputImage->m_data[rgbaIndex] = image_data[rgbIndex];     // R
                    outputImage->m_data[rgbaIndex + 1] = image_data[rgbIndex]; // G
                    outputImage->m_data[rgbaIndex + 2] = image_data[rgbIndex]; // B
                    outputImage->m_data[rgbaIndex + 3] = image_data[rgbIndex]; // A, full opacity
                }
            }
        }
        else if (channels == 2)
        {
            for (int y = 0; y < outputImage->m_height; ++y)
            {
                for (int x = 0; x < outputImage->m_width; ++x)
                {
                    size_t rgbIndex = (y * outputImage->m_width + x) * 4;  // RGB has 3 channels
                    size_t rgbaIndex = (y * outputImage->m_width + x) * 4; // RGBA has 4 channels

                    outputImage->m_data[rgbaIndex] = image_data[rgbIndex];         // R
                    outputImage->m_data[rgbaIndex + 1] = image_data[rgbIndex];     // G
                    outputImage->m_data[rgbaIndex + 2] = image_data[rgbIndex];     // B
                    outputImage->m_data[rgbaIndex + 3] = image_data[rgbIndex + 1]; // A, full opacity
                }
            }
        }
        else if (channels == 3)
        {
            for (int y = 0; y < outputImage->m_height; ++y)
            {
                for (int x = 0; x < outputImage->m_width; ++x)
                {
                    size_t rgbIndex = (y * outputImage->m_width + x) * 4;  // RGB has 3 channels
                    size_t rgbaIndex = (y * outputImage->m_width + x) * 4; // RGBA has 4 channels

                    outputImage->m_data[rgbaIndex] = image_data[rgbIndex + 2];     // R
                    outputImage->m_data[rgbaIndex + 1] = image_data[rgbIndex + 1]; // G
                    outputImage->m_data[rgbaIndex + 2] = image_data[rgbIndex];     // B
                    outputImage->m_data[rgbaIndex + 3] = 255;                      // A, full opacity
                }
            }
        }
        else if (channels == 4)
        {
            outputImage->m_data = image_data;
        }
        else
        {
            LOG_FATAL("ImageLoader_png: Unsupported number of channels: %d", channels);
        }
    }
    else
    {
        std::string errorText = std::string("Could not decode PNG: ") + std::string(lodepng_error_text(error));
        LOG_ERROR(errorText.c_str());
    }
}
} // namespace bow
