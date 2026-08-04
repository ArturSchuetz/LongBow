#include "Resources/Resources/BowMaterial.h"

#include "Resources/FileLoader/MeshLoader/BowModelLoader_obj.h"

#include "Platform/BowFileReader.h"


namespace bow
{

MaterialCollection::MaterialCollection(ResourceManager *creator, const std::string &name, ResourceHandle handle) : Resource(creator, name, handle), m_dataFromDisk(nullptr)
{
    FN("MaterialCollection::MaterialCollection");

    LOG_TRACE("Creating Material: %s", m_name.c_str());
}

MaterialCollection::~MaterialCollection()
{
    FN("MaterialCollection::~MaterialCollection");

    // have to call this here reather than in Resource destructor
    // since calling virtual methods in base destructors causes crash
    VUnload();
}

Material *MaterialCollection::CreateMaterial()
{
    FN("MaterialCollection::CreateMaterial");
    OPTICK_EVENT();

    Material *sub = new Material();
    sub->m_parent = this;

    m_materialList.push_back(sub);

    if (VIsLoaded())
        _dirtyState();

    return sub;
}

Material *MaterialCollection::CreateMaterial(const std::string &name)
{
    FN("MaterialCollection::CreateMaterial");
    OPTICK_EVENT();

    Material *sub = CreateMaterial();
    NameMaterial(name, (uint16_t)m_materialList.size() - 1);
    return sub;
}

void MaterialCollection::NameMaterial(const std::string &name, uint16_t index)
{
    FN("MaterialCollection::NameMaterial");
    OPTICK_EVENT();

    m_materialNameMap[name] = index;
}

void MaterialCollection::UnnameMaterial(const std::string &name)
{
    FN("MaterialCollection::UnnameMaterial");
    OPTICK_EVENT();

    auto i = m_materialNameMap.find(name);
    if (i != m_materialNameMap.end())
        m_materialNameMap.erase(i);
}

uint16_t MaterialCollection::GetMaterialIndex(const std::string &name) const
{
    FN("MaterialCollection::GetMaterialIndex");
    OPTICK_EVENT();

    auto i = m_materialNameMap.find(name);
    if (i == m_materialNameMap.end())
    {
        LOG_ERROR("MaterialCollection::GetMaterialIndex: No Material named "
                  "'%s' found.",
                  name);
        return -1;
    }

    return i->second;
}

Material *MaterialCollection::GetMaterial(const std::string &name) const
{
    FN("MaterialCollection::GetMaterial");
    OPTICK_EVENT();

    uint16_t index = GetMaterialIndex(name);
    if (index == (uint16_t)-1)
        return nullptr;

    return GetMaterial(index);
}

void MaterialCollection::DestroyMaterial(uint16_t index)
{
    FN("MaterialCollection::DestroyMaterial");
    OPTICK_EVENT();

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

void MaterialCollection::VPrepareImpl()
{
    FN("MaterialCollection::VPrepareImpl");

    // fully prebuffer into host RAM
    std::string filePath = VGetName();

    FileReader reader;

    if (reader.Open(filePath.c_str()))
    {
        m_sizeInBytes = reader.GetSizeOfFile();
        m_dataFromDisk = new char[m_sizeInBytes];
        m_dataFromDisk[m_sizeInBytes - 1] = '\0';

        uint32_t readedBytes = 0;
        char buffer[1024];

        reader.Seek(0);
        for (size_t i = 0; !reader.EndOfFile(); i += readedBytes)
        {
            readedBytes = (uint32_t)reader.Read(buffer, 1024);
            memcpy(m_dataFromDisk + i, buffer, readedBytes);
        }

        reader.Close();
    }
    else
    {
        LOG_ERROR("Could not open File '%s'!", filePath.c_str());
    }
}

void MaterialCollection::VUnprepareImpl()
{
    FN("MaterialCollection::VUnprepareImpl");

    if (m_dataFromDisk != nullptr)
    {
        delete[] m_dataFromDisk;
        m_dataFromDisk = nullptr;
    }
}

void MaterialCollection::VLoadImpl()
{
    FN("MaterialCollection::VLoadImpl");

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

void MaterialCollection::VPostLoadImpl() { FN("MaterialCollection::VPostLoadImpl"); }

void MaterialCollection::VUnloadImpl() { FN("MaterialCollection::VUnloadImpl"); }

} // namespace bow
