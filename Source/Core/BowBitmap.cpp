#pragma once
#include "BowBitmap.h"
#include "FreeImage.h"

namespace Bow {

	Bitmap::Bitmap() : dib(nullptr)
	{
		dib = std::shared_ptr<FIBITMAP>(nullptr);
	}

	Bitmap::~Bitmap()
	{
	}

	bool Bitmap::LoadFile( std::string FileName )
	{
		//image format
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	
		//check the file signature and deduce its format
		fif = FreeImage_GetFileType(FileName.c_str(), 0);

		//if still unknown, try to guess the file format from the file extension
		if(fif == FIF_UNKNOWN) 
			fif = FreeImage_GetFIFFromFilename(FileName.c_str());

		//if still unkown, return failure
		if(fif == FIF_UNKNOWN)
		{
			return false;
		}

		//check that the plugin has reading capabilities and load the file
		if(FreeImage_FIFSupportsReading(fif))
			dib = std::shared_ptr<FIBITMAP>(FreeImage_Load(fif, FileName.c_str()), [](FIBITMAP* dib){ FreeImage_Unload(dib); });

		//if the image failed to load, return failure
		if(!dib)
		{
			return false;
		}

		//get the image width and height
		m_width = FreeImage_GetWidth(dib.get());
		m_height = FreeImage_GetHeight(dib.get());
		m_size = FreeImage_GetDIBSize(dib.get());

		//if this somehow one of these failed (they shouldn't), return failure
		if((FreeImage_GetBits(dib.get()) == 0) || (m_width == 0) || (m_height == 0) || (m_size == 0))
		{
			return false;
		}

		//return success
		return true;
	}

	int Bitmap::GetWidth()
	{
		return m_width;
	}

	int Bitmap::GetHeight()
	{
		return m_height;
	}

	int Bitmap::GetSizeInBytes()
	{
		return m_size;
	}

	unsigned char* Bitmap::GetData()
	{
		return FreeImage_GetBits(dib.get());
	}

}