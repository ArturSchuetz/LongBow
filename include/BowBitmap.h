#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

struct FIBITMAP;

namespace Bow {
	namespace Core {

		class Bitmap
		{
		public:
			Bitmap();
			~Bitmap();

			bool LoadFile(std::string FileName);
			bool SaveFile(void* pixels, int width, int height, std::string fileName);

			int GetWidth() const;
			int GetHeight() const;
			int GetSizeInBytes() const;

			unsigned char* GetData() const;

		private:
			Bitmap(const Bitmap& other){} // do not copy
			unsigned int m_width;
			unsigned int m_height;
			unsigned int m_size;

			std::shared_ptr<FIBITMAP> dib;
		};

	}
}