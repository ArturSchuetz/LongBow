#pragma once
#include "LongBow.h"

struct FIBITMAP;

namespace Bow {

	struct Bitmap
    {
	public:
		Bitmap();
		~Bitmap();

		bool LoadFile( std::string FileName );

		int GetWidth();
		int GetHeight();
		int GetSizeInBytes();
		
		unsigned char* GetData();

	private:
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_size;

		std::shared_ptr<FIBITMAP> dib;
    };

}