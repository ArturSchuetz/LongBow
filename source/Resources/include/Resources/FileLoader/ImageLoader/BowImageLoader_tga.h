#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "Resources/Resources/BowImage.h"

namespace bow
{

class ImageLoader_tga
{
  public:
    ImageLoader_tga();
    ~ImageLoader_tga();

    void ImportImage(const char *inputData, Image *outputImage);

  private:
    // you shall not copy
    ImageLoader_tga(const ImageLoader_tga &other) = delete;
    ImageLoader_tga &operator=(const ImageLoader_tga &other) = delete;

    void LoadCompressedTGA(const char *inputData, Image *outputImage);

    uint32_t m_readCounter;
};
} // namespace bow