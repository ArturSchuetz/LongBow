#include "BowMaterial.h"

#include "BowResources.h"
#include "BowPlatform.h"
#include "BowCore.h"

namespace bow {

	Material::Material(ResourceManager* creator, const std::string& name, ResourceHandle handle)
		: Resource(creator, name, handle), m_dataFromDisk(nullptr)
	{
		LOG_TRACE("Creating Material: %s", name);
	}

	Material::~Material()
	{
		// have to call this here reather than in Resource destructor
		// since calling virtual methods in base destructors causes crash
		VUnload();
	}

	// ============================================================

	void Material::VPrepareImpl(void)
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

	void Material::VUnprepareImpl(void)
	{
		if (m_dataFromDisk != nullptr)
		{
			delete[] m_dataFromDisk;
			m_dataFromDisk = nullptr;
		}
	}

	void Material::VLoadImpl(void)
	{
		if (m_dataFromDisk == nullptr)
		{
			LOG_ERROR("Data doesn't appear to have been prepared in %s !", VGetName().c_str());
			return;
		}

		// Load stuff here
	}

	void Material::VPostLoadImpl(void)
	{

	}

	void Material::VUnloadImpl(void)
	{

	}

}
