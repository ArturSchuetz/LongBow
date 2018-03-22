#include "BowResources.h"
#include "BowImageLoader_bmp.h"

namespace bow {

	ImageLoader_bmp::ImageLoader_bmp()
	{

	}

	ImageLoader_bmp::~ImageLoader_bmp()
	{        

	}

	void ImageLoader_bmp::ImportImage(const char* inputData, Image* outputImage)
	{
		int headerSize;
		headerSize = *(int*)&inputData[14];
		outputImage->m_width = *(int*)&inputData[18];
		outputImage->m_height = *(int*)&inputData[22];
		outputImage->m_numBitsPerPixel = *(int*)&inputData[28];

		outputImage->m_sizeInBytes = (outputImage->m_numBitsPerPixel / (sizeof(unsigned char) * 8)) * outputImage->m_width * outputImage->m_height;
		outputImage->m_data.resize(outputImage->m_sizeInBytes);

		memcpy(&(outputImage->m_data[0]), inputData + headerSize, outputImage->m_sizeInBytes);

		for (unsigned int i = (unsigned int)headerSize; i+2 < (unsigned int)outputImage->m_sizeInBytes; i += 3)
		{
			unsigned char tmp = outputImage->m_data[i];
			outputImage->m_data[i] = outputImage->m_data[i + 2];
			outputImage->m_data[i + 2] = tmp;
		}
	}
}
