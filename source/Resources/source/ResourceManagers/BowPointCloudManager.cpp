#include "Resources/ResourceManagers/BowPointCloudManager.h"
#include "Resources/BowResources.h"


namespace bow
{

static std::shared_ptr<PointCloudManager> Instance;

PointCloudManager::PointCloudManager()
{
    FN("PointCloudManager::PointCloudManager");

    m_resourceType = "PointCloud";
}

PointCloudManager::~PointCloudManager() { FN("PointCloudManager::~PointCloudManager"); }

PointCloudManager &PointCloudManager::GetInstance()
{
    FN("PointCloudManager::GetInstance");
    OPTICK_EVENT();

    if (Instance.get() == nullptr)
    {
        Instance = std::shared_ptr<PointCloudManager>(new PointCloudManager());
        Instance->Initialize();
    }
    return *Instance.get();
}

void PointCloudManager::Initialize() { FN("PointCloudManager::Initialize"); }

PointCloudPtr PointCloudManager::Create(const std::string &name)
{
    FN("PointCloudManager::Create");
    OPTICK_EVENT();

    return std::static_pointer_cast<PointCloud>(CreateResource(name));
}

ResourcePtr PointCloudManager::CreateOrRetrieve(const std::string &filePath)
{
    FN("PointCloudManager::CreateOrRetrieve");
    OPTICK_EVENT();

    ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
    return res;
}

PointCloudPtr PointCloudManager::Prepare(const std::string &filePath)
{
    FN("PointCloudManager::Prepare");
    OPTICK_EVENT();

    PointCloudPtr pMesh = std::static_pointer_cast<PointCloud>(CreateOrRetrieve(filePath));
    pMesh->VPrepare();
    return pMesh;
}

PointCloudPtr PointCloudManager::Load(const std::string &filePath)
{
    FN("PointCloudManager::Load");
    OPTICK_EVENT();

    PointCloudPtr pMesh = std::static_pointer_cast<PointCloud>(CreateOrRetrieve(filePath));
    pMesh->VLoad();
    return pMesh;
}

PointCloudPtr PointCloudManager::CreateManual(const std::string &name)
{
    FN("PointCloudManager::CreateManual");
    OPTICK_EVENT();

    return Create(name);
}

Resource *PointCloudManager::VCreateImpl(const std::string &name, ResourceHandle handle)
{
    FN("PointCloudManager::VCreateImpl");
    OPTICK_EVENT();

    // no use for createParams here
    return new PointCloud((ResourceManager *)this, name, handle);
}
} // namespace bow
