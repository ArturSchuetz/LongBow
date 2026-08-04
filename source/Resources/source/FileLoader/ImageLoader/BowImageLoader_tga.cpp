#include "Resources/FileLoader/ImageLoader/BowImageLoader_tga.h"
#include "Resources/BowResources.h"

namespace bow
{

ImageLoader_tga::ImageLoader_tga() : m_readCounter(0) { FN("ImageLoader_tga::ImageLoader_tga"); }

ImageLoader_tga::~ImageLoader_tga() { FN("ImageLoader_tga::~ImageLoader_tga"); }

void ImageLoader_tga::ImportImage(const char *inputData, Image *outputImage)
{
    FN("ImageLoader_tga::ImportImage");
    OPTICK_EVENT();

    m_readCounter = 0;

    // Read the header of the TGA, compare it with the known headers for
    // compressed and uncompressed TGAs
    uint8_t ucHeader[18];
    memcpy(ucHeader, inputData + m_readCounter, sizeof(uint8_t) * 18);
    m_readCounter += sizeof(uint8_t) * 18;

    while (ucHeader[0] > 0)
    {
        --ucHeader[0];

        uint8_t temp;
        memcpy(&temp, inputData + m_readCounter, sizeof(uint8_t));
        m_readCounter += sizeof(uint8_t);
    }

    outputImage->m_width = ucHeader[13] * 256 + ucHeader[12];
    outputImage->m_height = ucHeader[15] * 256 + ucHeader[14];
    outputImage->m_numBitsPerPixel = ucHeader[16];
    outputImage->m_numberOfChannels = (outputImage->m_numBitsPerPixel / 8);
    outputImage->m_sizeInBytes = outputImage->m_numberOfChannels * sizeof(float) * outputImage->m_width * outputImage->m_height;

    // check whether width, height an BitsPerPixel are valid
    if ((outputImage->m_width <= 0) || (outputImage->m_height <= 0) || ((outputImage->m_numBitsPerPixel != 8) && (outputImage->m_numBitsPerPixel != 3 * 8) && (outputImage->m_numBitsPerPixel != 4 * 8)))
    {
        return;
    }

    uint32_t m_sizeInBytes = outputImage->m_width * outputImage->m_height * (outputImage->m_numBitsPerPixel / 8);

    // allocate the image-buffer
    std::vector<uint8_t> raw_data(outputImage->m_sizeInBytes);

    // call the appropriate loader-routine
    if (ucHeader[2] == 2)
    {
        memcpy(&raw_data[0], inputData + m_readCounter, outputImage->m_sizeInBytes);
    }
    else if (ucHeader[2] == 10)
    {
        LoadCompressedTGA(inputData, outputImage);
    }

    outputImage->m_data.resize(outputImage->m_numberOfChannels * outputImage->m_width * outputImage->m_height);
    for (size_t i = 0; i + outputImage->m_numberOfChannels - 1 < (uint32_t)(outputImage->m_numberOfChannels * outputImage->m_width * outputImage->m_height); i += outputImage->m_numberOfChannels)
    {
        for (size_t c = 0; c < outputImage->m_numberOfChannels; c++)
        {
            outputImage->m_data[i + (outputImage->m_numberOfChannels - c)] = (float)raw_data[i + c] / 255.0f;
        }
    }
}

void ImageLoader_tga::LoadCompressedTGA(const char *inputData, Image *outputImage)
{
    FN("ImageLoader_tga::LoadCompressedTGA");

    int iCurrentPixel = 0;

    const int iPixelCount = outputImage->m_width * outputImage->m_height;

    do
    {
        uint8_t ucChunkHeader = 0;

        memcpy(&ucChunkHeader, inputData + m_readCounter, sizeof(uint8_t));
        m_readCounter += sizeof(uint8_t);

        if (ucChunkHeader < 128)
        {
            // If the header is < 128, it means it is the number of RAW color
            // packets minus 1 that follow the header add 1 to get number of
            // following color values

            ucChunkHeader++;

            // Read RAW color values
            for (int i = 0; i < (int)ucChunkHeader; ++i)
            {
                memcpy(&outputImage->m_data[0] + (iCurrentPixel * (outputImage->m_numBitsPerPixel / 8)), inputData + m_readCounter, (outputImage->m_numBitsPerPixel / 8));
                m_readCounter += (outputImage->m_numBitsPerPixel / 8);

                ++iCurrentPixel;
            }
        }
        else // chunkheader > 128 RLE data, next color reapeated (chunkheader -
             // 127) times
        {
            ucChunkHeader -= 127; // Subteact 127 to get rid of the ID bit

            // copy the color into the image data as many times as dictated
            for (int i = 0; i < (int)ucChunkHeader; ++i)
            {
                // read the current color
                memcpy(&outputImage->m_data[0] + (iCurrentPixel * (outputImage->m_numBitsPerPixel / 8)), inputData + m_readCounter, (outputImage->m_numBitsPerPixel / 8));

                ++iCurrentPixel;
            }
            m_readCounter += (outputImage->m_numBitsPerPixel / 8);
        }
    } while (iCurrentPixel < iPixelCount);
}
} // namespace bow
