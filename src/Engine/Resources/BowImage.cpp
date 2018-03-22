#include "BowImage.h"

#include "BowResources.h"
#include "BowPlatform.h"
#include "BowCore.h"

#include "BowImageLoader_bmp.h"
#include "BowImageLoader_png.h"
#include "BowImageLoader_tga.h"

namespace bow {

	Image::Image(ResourceManager* creator, const std::string& name, ResourceHandle handle)
		: Resource(creator, name, handle), m_dataFromDisk(nullptr), m_width(0), m_height(0), m_sizeInBytes(0), m_numBitsPerPixel(0), m_data(0)
	{
		LOG_TRACE("Creating Image: %s", name);
	}

	Image::~Image()
	{
		// have to call this here reather than in Resource destructor
		// since calling virtual methods in base destructors causes crash
		VUnload();
	}

	unsigned int Image::GetSizeInBytes() {
		return m_sizeInBytes;
	}

	unsigned int Image::GetHeight() {
		return m_height;
	}

	unsigned int Image::GetWidth() {
		return m_width;
	}

	unsigned char* Image::GetData() {
		return &(m_data[0]);
	}

	// ============================================================

	void Image::VPrepareImpl(void)
	{
		// fully prebuffer into host RAM
		std::string filePath = VGetName();

		FileReader reader;

		if (reader.Open(filePath.c_str()))
		{
			m_sizeInBytes = reader.GetSizeOfFile();
			m_dataFromDisk = new char[m_sizeInBytes];
			m_dataFromDisk[m_sizeInBytes - 1] = '\0';

			unsigned int readedBytes = 0;
			char buffer[1024];

			reader.Seek(0);
			for (size_t i = 0; !reader.EndOfFile(); i += readedBytes)
			{
				readedBytes = reader.Read(buffer, 1024);
				memcpy(m_dataFromDisk + i, buffer, readedBytes);
			}

			reader.Close();
		}
		else
		{
			LOG_ERROR("Could not open File '%s'!", filePath.c_str());
		}
	}

	void Image::VUnprepareImpl(void)
	{
		if (m_dataFromDisk != nullptr)
		{
			delete[] m_dataFromDisk;
			m_dataFromDisk = nullptr;
		}
	}

	void Image::VLoadImpl(void)
	{
		if (m_dataFromDisk == nullptr)
		{
			LOG_ERROR("Data doesn't appear to have been prepared in %s !", VGetName().c_str());
			return;
		}

		std::string filePath = VGetName();
		size_t pos = filePath.find_last_of(".");
		if (pos >= 0)
		{
			std::string extension = filePath.substr(pos + 1);

			if (extension == "bmp")
			{
				ImageLoader_bmp loader;
				loader.ImportImage(m_dataFromDisk, this);
			}
			else if (extension == "png")
			{
				ImageLoader_png loader;
				std::vector<unsigned char> raw_data(m_dataFromDisk, m_dataFromDisk + m_sizeInBytes);
				loader.ImportImage(raw_data, this);
			}
			else if (extension == "tga")
			{
				ImageLoader_tga loader;
				loader.ImportImage(m_dataFromDisk, this);
			}
			else
			{
				LOG_ERROR("Unknown file format for image");
			}
		}
	}

	void Image::VPostLoadImpl(void)
	{

	}

	void Image::VUnloadImpl(void)
	{
		m_width = 0;
		m_height = 0;
		m_sizeInBytes = 0;
		m_numBitsPerPixel = 0;
		m_data.clear();
	}

}
