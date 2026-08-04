#include "Resources/FileLoader/ImageLoader/BowImageLoader_hdr.h"
#include "Resources/BowResources.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>

union RGBe
{
    struct
    {
        uint8_t r, g, b, e;
    };
    uint8_t v[4];
};

inline void RGBEtoFloats(const RGBe &RV, float *FV, float inv_img_exposure)
{
    FN("RGBEtoFloats");

    if (RV.e == 0)
        FV[0] = FV[1] = FV[2] = 0.0f;
    else
    {
        const int HDR_EXPON_BIAS = 128;
        float s = (float)ldexp(1.0, (int(RV.e) - (HDR_EXPON_BIAS + 8)));
        s *= inv_img_exposure;
        FV[0] = (RV.r + 0.5f) * s;
        FV[1] = (RV.g + 0.5f) * s;
        FV[2] = (RV.b + 0.5f) * s;
    }
}

void ReadScanlineNoRLE(std::istringstream &inf, RGBe *RGBEline, const size_t wid)
{
    FN("ReadScanlineNoRLE");

    inf.read(reinterpret_cast<char *>(RGBEline), wid * sizeof(RGBe));

    if (inf.eof())
    {
        LOG_ERROR("Premature file end in ReadScanlineNoRLE");
        return;
    }
}

void ReadScanline(std::istringstream &inf, RGBe *RGBEline, const size_t wid)
{
    FN("ReadScanline");

    const size_t MinLen = 8, MaxLen = 0x7fff;

    if (wid < MinLen || wid > MaxLen)
        return ReadScanlineNoRLE(inf, RGBEline, wid);

    char c0, c1, c2, c3;
    inf.get(c0);
    inf.get(c1);
    inf.get(c2);
    inf.get(c3);

    if (inf.eof())
    {
        LOG_ERROR("Premature file end in ReadScanline 1");
        return;
    }

    if (c0 != 2 || c1 != 2 || (c2 & 0x80))
    {
        inf.putback(c3);
        inf.putback(c2);
        inf.putback(c1);
        inf.putback(c0);
        return ReadScanlineNoRLE(inf, RGBEline,
                                 wid); // Found an old-format scanline
    }

    if (size_t(size_t(c2) << 8 | size_t(c3)) != wid)
    {
        LOG_ERROR("Scanline width inconsistent");
        return;
    }

    // This scanline is RLE.
    for (size_t ch = 0; ch < 4; ch++)
    {
        for (size_t x = 0; x < wid;)
        {
            uint8_t code;
            inf.get(reinterpret_cast<char &>(code));

            if (inf.eof())
            {
                LOG_ERROR("Premature file end in ReadScanline 2");
                return;
            }

            if (code > 0x80)
            { // RLE span
                char pix;
                inf.get(pix);

                if (inf.eof())
                {
                    LOG_ERROR("Premature file end in ReadScanline 3");
                    return;
                }

                code = code & 0x7f;

                while (code--)
                    RGBEline[x++].v[ch] = pix;
            }
            else
            { // Arbitrary span
                while (code--)
                {
                    inf.get(reinterpret_cast<char &>(RGBEline[x++].v[ch]));

                    if (inf.eof())
                    {
                        LOG_ERROR("Premature file end in ReadScanline 4");
                        return;
                    }
                }
            }
        }
    }
}

namespace bow
{

ImageLoader_hdr::ImageLoader_hdr() { FN("ImageLoader_hdr::ImageLoader_hdr"); }

ImageLoader_hdr::~ImageLoader_hdr() { FN("ImageLoader_hdr::~ImageLoader_hdr"); }

void ImageLoader_hdr::ImportImage(const char *inputData, Image *outputImage)
{
    FN("ImageLoader_hdr::ImportImage");
    OPTICK_EVENT();

    std::string magic, comment;
    float exposure = 1.0f;

    std::string linebuf;
    std::istringstream dataStream((const char *)&inputData[0]);

    std::getline(dataStream, magic);
    if (magic != "#?RADIANCE" && magic != "#?RGBE")
    {
        LOG_ERROR("File isn't Radiance.");
        return;
    }

    for (;;)
    {
        std::getline(dataStream, comment);

        // VS2010 doesn't let you look at the 0th element of a 0 length string,
        // so this was tripping debug asserts
        if (comment.empty())
            break;
        if (comment[0] == '#')
            continue;

        if (comment.find("FORMAT") != std::string::npos)
        {
            if (comment == "FORMAT=32-bit_rle_rgbe")
            {
                outputImage->m_numBitsPerPixel = sizeof(float) * 4;
            }
            else
            {
                LOG_ERROR("Can only handle RGBe, not XYZe.");
                return;
            }
            continue;
        }

        size_t ofs = comment.find("EXPOSURE=");
        if (ofs != std::string::npos)
        {
            exposure = (float)atof(comment.c_str() + ofs + 9);
        }
    }

    std::string major, minor;
    dataStream >> minor >> outputImage->m_height >> major >> outputImage->m_width;

    if (minor != "-Y" || major != "+X")
    {
        LOG_ERROR("Can only handle -Y +X ordering");
        return;
    }

    if (outputImage->m_width <= 0 || outputImage->m_height <= 0)
    {
        LOG_ERROR("Invalid image dimensions");
        return;
    }

    std::getline(dataStream, comment); // Read the last newline of the header

    RGBe *RGBERaster = new RGBe[outputImage->m_width * outputImage->m_height];
    for (size_t y = 0; y < outputImage->m_height; y++)
    {
        ReadScanline(dataStream, RGBERaster + outputImage->m_width * y, outputImage->m_width);
    }

    outputImage->m_numberOfChannels = 4;
    outputImage->m_sizeInBytes = 4 * sizeof(float) * outputImage->m_width * outputImage->m_height;
    outputImage->m_data.resize(outputImage->m_numberOfChannels * outputImage->m_width * outputImage->m_height);

    float inv_img_exposure = 1.0f / exposure;
    for (size_t i = 0; i < outputImage->m_width * outputImage->m_height; i++)
    {
        RGBEtoFloats(RGBERaster[i], ((float *)(&outputImage->m_data[0])) + (i * 4), inv_img_exposure);
    }
    delete[] RGBERaster;
}
} // namespace bow
