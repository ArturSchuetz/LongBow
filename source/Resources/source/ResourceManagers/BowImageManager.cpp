#include "Resources/ResourceManagers/BowImageManager.h"
#include "Resources/BowResources.h"



namespace bow
{

static std::shared_ptr<ImageManager> Instance;

ImageManager::ImageManager()
{
    FN("ImageManager::~ImageManager");

    m_resourceType = "Image";
}

ImageManager::~ImageManager() { FN("ImageManager::~ImageManager"); }

ImageManager &ImageManager::GetInstance()
{
    FN("ImageManager::GetInstance");
    OPTICK_EVENT();

    if (Instance.get() == nullptr)
    {
        Instance = std::shared_ptr<ImageManager>(new ImageManager());
        Instance->Initialize();
    }
    return *Instance.get();
}

void ImageManager::Initialize() { FN("ImageManager::Initialize"); }

ImagePtr ImageManager::Create(const std::string &name)
{
    FN("ImageManager::Create");
    OPTICK_EVENT();

    return std::static_pointer_cast<Image>(CreateResource(name));
}

ResourcePtr ImageManager::CreateOrRetrieve(const std::string &filePath)
{
    FN("ImageManager::CreateOrRetrieve");
    OPTICK_EVENT();

    ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
    return res;
}

ImagePtr ImageManager::Prepare(const std::string &filePath)
{
    FN("ImageManager::Prepare");
    OPTICK_EVENT();

    ImagePtr pImage = std::static_pointer_cast<Image>(CreateOrRetrieve(filePath));
    pImage->VPrepare();
    return pImage;
}

ImagePtr ImageManager::Load(const std::string &filePath)
{
    FN("ImageManager::Load");
    OPTICK_EVENT();

    ImagePtr pImage = std::static_pointer_cast<Image>(CreateOrRetrieve(filePath));
    pImage->VLoad();
    return pImage;
}

ImagePtr ImageManager::CreateManual(const std::string &name)
{
    FN("ImageManager::CreateManual");
    OPTICK_EVENT();

    return Create(name);
}

Resource *ImageManager::VCreateImpl(const std::string &name, ResourceHandle handle)
{
    FN("ImageManager::VCreateImpl");
    OPTICK_EVENT();

    // no use for createParams here
    return new Image((ResourceManager *)this, name, handle);
}
} // namespace bow
