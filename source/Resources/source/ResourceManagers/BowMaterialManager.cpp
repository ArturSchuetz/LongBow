#include "Resources/ResourceManagers/BowMaterialManager.h"
#include "Resources/BowResources.h"



namespace bow
{

static std::shared_ptr<MaterialManager> Instance;

MaterialManager::MaterialManager()
{
    FN("MaterialManager::MaterialManager");

    m_resourceType = "Material";
}

MaterialManager::~MaterialManager() { FN("MaterialManager::~MaterialManager"); }

MaterialManager &MaterialManager::GetInstance()
{
    FN("MaterialManager::GetInstance");
    OPTICK_EVENT();

    if (Instance.get() == nullptr)
    {
        Instance = std::shared_ptr<MaterialManager>(new MaterialManager());
        Instance->Initialize();
    }
    return *Instance.get();
}

void MaterialManager::Initialize() { FN("MaterialManager::Initialize"); }

MaterialCollectionPtr MaterialManager::Create(const std::string &name)
{
    FN("MaterialManager::Create");
    OPTICK_EVENT();

    return std::static_pointer_cast<MaterialCollection>(CreateResource(name));
}

ResourcePtr MaterialManager::CreateOrRetrieve(const std::string &filePath)
{
    FN("MaterialManager::CreateOrRetrieve");
    OPTICK_EVENT();

    ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
    return res;
}

MaterialCollectionPtr MaterialManager::Prepare(const std::string &filePath)
{
    FN("MaterialManager::Prepare");
    OPTICK_EVENT();

    MaterialCollectionPtr pMaterial = std::static_pointer_cast<MaterialCollection>(CreateOrRetrieve(filePath));
    pMaterial->VPrepare();
    return pMaterial;
}

MaterialCollectionPtr MaterialManager::Load(const std::string &filePath)
{
    FN("MaterialManager::Load");
    OPTICK_EVENT();

    MaterialCollectionPtr pMaterial = std::static_pointer_cast<MaterialCollection>(CreateOrRetrieve(filePath));
    pMaterial->VLoad();
    return pMaterial;
}

MaterialCollectionPtr MaterialManager::CreateManual(const std::string &name)
{
    FN("MaterialManager::CreateManual");
    OPTICK_EVENT();

    return Create(name);
}

Resource *MaterialManager::VCreateImpl(const std::string &name, ResourceHandle handle)
{
    FN("MaterialManager::VCreateImpl");
    OPTICK_EVENT();

    // no use for createParams here
    return new MaterialCollection((ResourceManager *)this, name, handle);
}
} // namespace bow
