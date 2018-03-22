#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowPlatformPredeclares.h"

#include "BowMath.h"
#include "BowImage.h"

#include <Windows.h>

namespace bow {

	class ImageLoader_png
	{
	public:
		ImageLoader_png();
		~ImageLoader_png();

		void ImportImage(const std::vector<unsigned char>& inputData, Image* outputImage);

	private:
	};
}