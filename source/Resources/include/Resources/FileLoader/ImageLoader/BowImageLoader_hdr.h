#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "Resources/Resources/BowImage.h"

namespace bow
{

class ImageLoader_hdr
{
  public:
    ImageLoader_hdr();
    ~ImageLoader_hdr();

    void ImportImage(const char *inputData, Image *outputImage);

  private:
    // you shall not copy
    ImageLoader_hdr(const ImageLoader_hdr &other) = delete;
    ImageLoader_hdr &operator=(const ImageLoader_hdr &other) = delete;
};
} // namespace bow