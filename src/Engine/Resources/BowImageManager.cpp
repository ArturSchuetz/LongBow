#include "BowResources.h"
#include "BowImageManager.h"

namespace bow {
	
	static std::shared_ptr<ImageManager> Instance;

	ImageManager::ImageManager()
	{
		m_resourceType = "Image";
	}

	ImageManager::~ImageManager()
	{
	}

	ImageManager& ImageManager::GetInstance()
	{
		if (Instance.get() == nullptr)
		{
			Instance = std::shared_ptr<ImageManager>(new ImageManager());
			Instance->Initialize();
		}
		return *Instance.get();
	}

	void ImageManager::Initialize(void)
	{

	}

	ImagePtr ImageManager::Create(const std::string& name)
	{
		return std::static_pointer_cast<Image>(CreateResource(name));
	}

	ResourcePtr ImageManager::CreateOrRetrieve(const std::string& filePath)
	{
		ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
		return res;
	}

	ImagePtr ImageManager::Prepare(const std::string& filePath)
	{
		ImagePtr pImage = std::static_pointer_cast<Image>(CreateOrRetrieve(filePath));
		pImage->VPrepare();
		return pImage;
	}

	ImagePtr ImageManager::Load(const std::string& filePath)
	{
		ImagePtr pImage = std::static_pointer_cast<Image>(CreateOrRetrieve(filePath));
		pImage->VLoad();
		return pImage;
	}

	ImagePtr ImageManager::CreateManual(const std::string& name)
	{
		return Create(name);
	}

	Resource* ImageManager::VCreateImpl(const std::string& name, ResourceHandle handle)
	{
		// no use for createParams here
		return new Image((ResourceManager*)this, name, handle);
	}
}
