#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "Resources/BowResource.h"

#include <vector>

namespace bow
{
class RESOURCES_API Image : public Resource
{
    friend class ImageLoader_bmp;
    friend class ImageLoader_hdr;
    friend class ImageLoader_png;
    friend class ImageLoader_tga;

  public:
    enum class Format : uint8_t
    {
        uchar,
        ushort,
        float32
    };

    Image(ResourceManager *creator, const std::string &name, ResourceHandle handle);
    ~Image();

    uint32_t GetHeight();
    uint32_t GetWidth();
    uint32_t GetBitsPerPixel();
    uint32_t GetNumChannels();
    Format GetFormat();
    unsigned char *GetData();

  private:
    // you shall not copy
    Image(const Image &other) = delete;
    Image &operator=(const Image &other) = delete;

    /** Loads the image from disk.  This call only performs IO, it
    does not parse the bytestream or check for any errors therein.
    You have to call load() to do that.
    */
    void VPrepareImpl();

    /** Destroys data cached by prepareImpl.
     */
    void VUnprepareImpl();

    /// @copydoc Resource::VLoadImpl
    void VLoadImpl();

    /// @copydoc Resource::VPostLoadImpl
    void VPostLoadImpl();

    /// @copydoc Resource::VUnloadImpl
    void VUnloadImpl();

    char *m_dataFromDisk;

    int32_t m_width;
    int32_t m_height;
    int16_t m_numberOfChannels;
    int16_t m_numBitsPerPixel;
    Format m_format;
    std::vector<unsigned char> m_data;
};

} // namespace bow
