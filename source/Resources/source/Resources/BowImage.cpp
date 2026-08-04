#include "Resources/Resources/BowImage.h"

#include "Resources/FileLoader/ImageLoader/BowImageLoader_bmp.h"
#include "Resources/FileLoader/ImageLoader/BowImageLoader_hdr.h"
#include "Resources/FileLoader/ImageLoader/BowImageLoader_png.h"
#include "Resources/FileLoader/ImageLoader/BowImageLoader_tga.h"

#include "CoreSystems/BowLogger.h"


#include "Platform/BowFileReader.h"

namespace bow
{

Image::Image(ResourceManager *creator, const std::string &name, ResourceHandle handle)
    : Resource(creator, name, handle), m_dataFromDisk(nullptr), m_width(0), m_height(0), m_numberOfChannels(0), m_numBitsPerPixel(0), m_format(Format::uchar), m_data(0)
{
    FN("ImageImage::Image");

    LOG_TRACE("Creating Image: %s", name.c_str());
}

Image::~Image()
{
    FN("Image::~Image");

    // have to call this here reather than in Resource destructor
    // since calling virtual methods in base destructors causes crash
    VUnload();
}

uint32_t Image::GetHeight()
{
    FN("Image::GetHeight");
    OPTICK_EVENT();

    return m_height;
}

uint32_t Image::GetWidth()
{
    FN("Image::GetWidth");
    OPTICK_EVENT();

    return m_width;
}

uint32_t Image::GetBitsPerPixel()
{
    FN("Image::GetBitsPerPixel");
    OPTICK_EVENT();

    return m_numBitsPerPixel;
}

uint32_t Image::GetNumChannels()
{
    FN("Image::GetNumChannels");
    OPTICK_EVENT();

    return m_numberOfChannels;
}

Image::Format Image::GetFormat()
{
    FN("Image::GetFormat");
    OPTICK_EVENT();

    return m_format;
}

unsigned char *Image::GetData()
{
    FN("Image::GetData");
    OPTICK_EVENT();

    return &(m_data[0]);
}

// ============================================================

void Image::VPrepareImpl()
{
    FN("Image::VPrepareImpl");

    // fully prebuffer into host RAM
    std::string filePath = VGetName();

    FileReader reader;

    if (reader.Open(filePath.c_str()))
    {
        m_sizeInBytes = reader.GetSizeOfFile();
        m_dataFromDisk = new char[m_sizeInBytes];
        m_dataFromDisk[m_sizeInBytes - 1] = '\0';

        uint32_t readedBytes = 0;
        char buffer[1024];

        reader.Seek(0);
        for (size_t i = 0; !reader.EndOfFile(); i += readedBytes)
        {
            readedBytes = (uint32_t)reader.Read(buffer, 1024);
            memcpy(m_dataFromDisk + i, buffer, readedBytes);
        }

        reader.Close();
    }
    else
    {
        LOG_ERROR("Could not open File '%s'!", filePath.c_str());
    }
}

void Image::VUnprepareImpl()
{
    FN("Image::VUnprepareImpl");

    if (m_dataFromDisk != nullptr)
    {
        delete[] m_dataFromDisk;
        m_dataFromDisk = nullptr;
    }
}

void Image::VLoadImpl()
{
    FN("Image::VLoadImpl");

    if (m_dataFromDisk == nullptr)
    {
        LOG_ERROR("Data doesn't appear to have been prepared in %s !", VGetName().c_str());
        return;
    }

    std::string filePath = VGetName();
    size_t pos = filePath.find_last_of(".");
    if (pos >= 0)
    {
        std::string extension = filePath.substr(pos + 1);

        if (extension == "bmp")
        {
            ImageLoader_bmp loader;
            loader.ImportImage(m_dataFromDisk, this);
        }
        else if (extension == "hdr" || extension == "HDR")
        {
            ImageLoader_hdr loader;
            loader.ImportImage(m_dataFromDisk, this);
        }
        else if (extension == "png")
        {
            ImageLoader_png loader;
            std::vector<uint8_t> raw_data(m_dataFromDisk, m_dataFromDisk + m_sizeInBytes);
            loader.ImportImage(raw_data, this);
        }
        else if (extension == "tga")
        {
            ImageLoader_tga loader;
            loader.ImportImage(m_dataFromDisk, this);
        }
        else
        {
            LOG_ERROR("Unknown file format for image");
        }
    }
}

void Image::VPostLoadImpl() { FN("Image::VPostLoadImpl"); }

void Image::VUnloadImpl()
{
    FN("Image::VUnloadImpl");

    m_width = 0;
    m_height = 0;
    m_sizeInBytes = 0;
    m_numBitsPerPixel = 0;
    m_data.clear();
}

} // namespace bow
