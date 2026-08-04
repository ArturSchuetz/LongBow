#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "Resources/Resources/BowImage.h"

namespace bow
{

class ImageLoader_bmp
{
  public:
    ImageLoader_bmp();
    ~ImageLoader_bmp();

    void ImportImage(const char *inputData, Image *outputImage);

  private:
    // you shall not copy
    ImageLoader_bmp(const ImageLoader_bmp &other) = delete;
    ImageLoader_bmp &operator=(const ImageLoader_bmp &other) = delete;
};
} // namespace bow