#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowPlatformPredeclares.h"

#include "BowMath.h"
#include "BowImage.h"

namespace bow {

	class ImageLoader_bmp
	{
	public:
		ImageLoader_bmp();
		~ImageLoader_bmp();

		void ImportImage(const char* inputData, Image* outputImage);

	private:
	};
}