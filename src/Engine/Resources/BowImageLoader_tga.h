#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowPlatformPredeclares.h"

#include "BowMath.h"
#include "BowImage.h"

namespace bow {

	class ImageLoader_tga
	{
	public:
		ImageLoader_tga();
		~ImageLoader_tga();

		void ImportImage(const char* inputData, Image* outputImage);

	private:
		void LoadCompressedTGA(const char* inputData, Image* outputImage);

		unsigned int m_readCounter;
	};
}