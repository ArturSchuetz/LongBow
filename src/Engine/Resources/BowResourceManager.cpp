#include "BowResources.h"
#include "BowResourceManager.h"

namespace Bow {
	namespace Core {

		static std::shared_ptr<ResourceManager> Instance;

		ResourceManager::ResourceManager()
		{

		}

		ResourceManager::~ResourceManager()
		{
			Release();
		}

		void ResourceManager::Release()
		{

		}

		ResourceManager& ResourceManager::GetInstance()
		{
			if (Instance.get() == nullptr)
			{
				Instance = std::shared_ptr<ResourceManager>(new ResourceManager());
			}
			return *Instance.get();
		}

		Bitmap* ResourceManager::LoadImageFile(const std::string& path)
		{
			if (Images.find(path) == Images.end())
			{
				Images[path] = Bitmap();
				if (!Images[path].LoadFile(path))
				{
					return nullptr;
				}
			}
			return &Images[path];
		}
	}
}