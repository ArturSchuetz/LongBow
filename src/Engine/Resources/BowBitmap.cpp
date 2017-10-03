#include "BowResources.h"
#include "BowBitmap.h"
#include "FreeImage.h"

namespace Bow {
	namespace Core {

		Bitmap::Bitmap() : dib(nullptr)
		{
			dib = std::shared_ptr<FIBITMAP>(nullptr);
		}

		Bitmap::~Bitmap()
		{
		}

		bool Bitmap::LoadFile(std::string FileName)
		{
			//image format
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

			//check the file signature and deduce its format
			fif = FreeImage_GetFileType(FileName.c_str(), 0);

			//if still unknown, try to guess the file format from the file extension
			if (fif == FIF_UNKNOWN)
				fif = FreeImage_GetFIFFromFilename(FileName.c_str());

			//if still unkown, return failure
			if(fif == FIF_UNKNOWN)
			{
				return false;
			}

			//check that the plugin has reading capabilities and load the file
			if (FreeImage_FIFSupportsReading(fif))
				dib = std::shared_ptr<FIBITMAP>(FreeImage_Load(fif, FileName.c_str()), [](FIBITMAP* dib){ FreeImage_Unload(dib); });

			//if the image failed to load, return failure
			if (!dib)
			{
				return false;
			}

			//get the image width and height
			m_width = FreeImage_GetWidth(dib.get());
			m_height = FreeImage_GetHeight(dib.get());
			m_size = FreeImage_GetDIBSize(dib.get());

			//if this somehow one of these failed (they shouldn't), return failure
			if ((FreeImage_GetBits(dib.get()) == 0) || (m_width == 0) || (m_height == 0) || (m_size == 0))
			{
				return false;
			}

			//return success
			return true;
		}

		bool Bitmap::SaveFile(void* pixels, int width, int height, std::string fileName)
		{
			// Convert to FreeImage format & save to file
			FIBITMAP* image = FreeImage_ConvertFromRawBits((BYTE*)pixels, width, height, 3 * width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
			FreeImage_Save(FIF_BMP, image, fileName.c_str(), 0);

			// Free resources
			FreeImage_Unload(image);

			//return success
			return true;
		}


		int Bitmap::GetWidth() const
		{
			return m_width;
		}

		int Bitmap::GetHeight() const
		{
			return m_height;
		}

		int Bitmap::GetSizeInBytes() const
		{
			return m_size;
		}

		unsigned char* Bitmap::GetData() const
		{
			return FreeImage_GetBits(dib.get());
		}

	}
}