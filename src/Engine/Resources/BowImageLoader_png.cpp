#include "BowResources.h"
#include "BowImageLoader_png.h"

#include "LodePNG\lodepng.h"

namespace bow {

	ImageLoader_png::ImageLoader_png()
	{

	}

	ImageLoader_png::~ImageLoader_png()
	{        

	}

	void ImageLoader_png::ImportImage(const std::vector<unsigned char>& inputData, Image* outputImage)
	{
		lodepng::State state; //optionally customize this one

		unsigned int width, height;
		unsigned error = lodepng::decode(outputImage->m_data, width, height, state, inputData);
		if (!error)
		{
			outputImage->m_width = width;
			outputImage->m_height = height;
			outputImage->m_sizeInBytes = outputImage->m_data.size();
			unsigned int channels = -1;
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
			}
			outputImage->m_numBitsPerPixel = state.info_raw.bitdepth * channels;
		}
		else
		{
			std::string errorText = std::string("Could not decode PNG: ") + std::string(lodepng_error_text(error));
			LOG_ERROR(errorText.c_str());
		}
	}
}
