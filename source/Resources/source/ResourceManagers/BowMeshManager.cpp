#include "Resources/ResourceManagers/BowMeshManager.h"
#include "Resources/BowResources.h"



namespace bow
{

static std::shared_ptr<MeshManager> Instance;

MeshManager::MeshManager()
{
    FN("MeshManager::MeshManager");

    m_resourceType = "Mesh";
}

MeshManager::~MeshManager() { FN("MeshManager::~MeshManager"); }

MeshManager &MeshManager::GetInstance()
{
    FN("MeshManager::GetInstance");
    OPTICK_EVENT();

    if (Instance.get() == nullptr)
    {
        Instance = std::shared_ptr<MeshManager>(new MeshManager());
        Instance->Initialize();
    }
    return *Instance.get();
}

void MeshManager::Initialize() { FN("MeshManager::Initialize"); }

MeshPtr MeshManager::Create(const std::string &name)
{
    FN("MeshManager::Create");
    OPTICK_EVENT();

    return std::static_pointer_cast<Mesh>(CreateResource(name));
}

ResourcePtr MeshManager::CreateOrRetrieve(const std::string &filePath)
{
    FN("MeshManager::CreateOrRetrieve");
    OPTICK_EVENT();

    ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
    return res;
}

MeshPtr MeshManager::Prepare(const std::string &filePath)
{
    FN("MeshManager::Prepare");
    OPTICK_EVENT();

    MeshPtr pMesh = std::static_pointer_cast<Mesh>(CreateOrRetrieve(filePath));
    pMesh->VPrepare();
    return pMesh;
}

MeshPtr MeshManager::Load(const std::string &filePath)
{
    FN("MeshManager::Load");
    OPTICK_EVENT();

    MeshPtr pMesh = std::static_pointer_cast<Mesh>(CreateOrRetrieve(filePath));
    pMesh->VLoad();
    return pMesh;
}

MeshPtr MeshManager::CreateManual(const std::string &name)
{
    FN("MeshManager::CreateManual");
    OPTICK_EVENT();

    return Create(name);
}

Resource *MeshManager::VCreateImpl(const std::string &name, ResourceHandle handle)
{
    FN("MeshManager::VCreateImpl");
    OPTICK_EVENT();

    // no use for createParams here
    return new Mesh((ResourceManager *)this, name, handle);
}

} // namespace bow
