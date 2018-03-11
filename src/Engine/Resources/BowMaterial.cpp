#include "BowMaterial.h"

#include "BowResources.h"
#include "BowPlatform.h"
#include "BowCore.h"

#include "BowModelLoader_obj.h"

namespace bow {

	MaterialCollection::MaterialCollection(ResourceManager* creator, const std::string& name, ResourceHandle handle)
		: Resource(creator, name, handle)
		, m_dataFromDisk(nullptr)
	{
		LOG_TRACE("Creating Material: %s", m_name.c_str());
	}

	MaterialCollection::~MaterialCollection()
	{
		// have to call this here reather than in Resource destructor
		// since calling virtual methods in base destructors causes crash
		VUnload();
	}

	Material* MaterialCollection::CreateMaterial(void)
	{
		Material* sub = new Material();
		sub->m_parent = this;

		m_materialList.push_back(sub);

		if (VIsLoaded())
			_dirtyState();

		return sub;
	}

	Material* MaterialCollection::CreateMaterial(const std::string& name)
	{
		Material *sub = CreateMaterial();
		NameMaterial(name, (unsigned short)m_materialList.size() - 1);
		return sub;
	}

	void MaterialCollection::NameMaterial(const std::string& name, unsigned short index)
	{
		m_materialNameMap[name] = index;
	}

	void MaterialCollection::UnnameMaterial(const std::string& name)
	{
		auto i = m_materialNameMap.find(name);
		if (i != m_materialNameMap.end())
			m_materialNameMap.erase(i);
	}

	unsigned short MaterialCollection::GetMaterialIndex(const std::string& name) const
	{
		auto i = m_materialNameMap.find(name);
		if (i == m_materialNameMap.end())
		{
			LOG_ERROR("MaterialCollection::GetMaterialIndex: No Material named '%s' found.", name);
		}

		return i->second;
	}

	Material* MaterialCollection::GetMaterial(const std::string& name) const
	{
		unsigned short index = GetMaterialIndex(name);
		return GetMaterial(index);
	}

	void MaterialCollection::DestroyMaterial(unsigned short index)
	{
		if (index >= m_materialList.size())
		{
			LOG_ERROR("MaterialCollection::DestroyMaterial: Index out of bounds.");
		}
		auto it = m_materialList.begin();
		std::advance(it, index);
		m_materialList.erase(it);

		// Fix up any name/index entries
		for (auto ni = m_materialNameMap.begin(); ni != m_materialNameMap.end();)
		{
			if (ni->second == index)
			{
				auto eraseIt = ni++;
				m_materialNameMap.erase(eraseIt);
			}
			else
			{
				// reduce indexes following
				if (ni->second > index)
					ni->second = ni->second - 1;

				++ni;
			}
		}

		if (VIsLoaded())
			_dirtyState();
	}

	// ============================================================

	void MaterialCollection::VPrepareImpl(void)
	{
		// fully prebuffer into host RAM
		std::string filePath = VGetName();

		FileReader reader;

		if (reader.Open(filePath.c_str()))
		{
			size_t sizeInBytes = reader.GetSizeOfFile();
			m_dataFromDisk = new char[sizeInBytes];
			m_dataFromDisk[sizeInBytes - 1] = '\0';
				
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

	void MaterialCollection::VUnprepareImpl(void)
	{
		if (m_dataFromDisk != nullptr)
		{
			delete[] m_dataFromDisk;
			m_dataFromDisk = nullptr;
		}
	}

	void MaterialCollection::VLoadImpl(void)
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
			std::string extension = filePath.substr(pos +1);

			if (extension == "mtl") 
			{
				ModelLoader_obj loader;
				loader.ImportMaterial(m_dataFromDisk, this);
			}
			else
			{
				LOG_ERROR("Unknown file format for material");
			}
		}
	}

	void MaterialCollection::VPostLoadImpl(void)
	{

	}

	void MaterialCollection::VUnloadImpl(void)
	{

	}
}
