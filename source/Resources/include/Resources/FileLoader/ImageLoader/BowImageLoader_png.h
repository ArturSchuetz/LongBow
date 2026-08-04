#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "Resources/Resources/BowImage.h"

namespace bow
{

class ImageLoader_png
{
  public:
    ImageLoader_png();
    ~ImageLoader_png();

    void ImportImage(const std::vector<unsigned char> &inputData, Image *outputImage);

  private:
    // you shall not copy
    ImageLoader_png(const ImageLoader_png &other) = delete;
    ImageLoader_png &operator=(const ImageLoader_png &other) = delete;
};
} // namespace bow