#include "BowResources.h"
#include "BowPointCloudManager.h"

namespace bow {
	
	static std::shared_ptr<PointCloudManager> Instance;

	PointCloudManager::PointCloudManager()
	{
		m_resourceType = "PointCloud";
	}

	PointCloudManager::~PointCloudManager()
	{
	}

	PointCloudManager& PointCloudManager::GetInstance()
	{
		if (Instance.get() == nullptr)
		{
			Instance = std::shared_ptr<PointCloudManager>(new PointCloudManager());
			Instance->Initialize();
		}
		return *Instance.get();
	}

	void PointCloudManager::Initialize(void)
	{

	}

	PointCloudPtr PointCloudManager::Create(const std::string& name)
	{
		return std::static_pointer_cast<PointCloud>(CreateResource(name));
	}

	ResourcePtr PointCloudManager::CreateOrRetrieve(const std::string& filePath)
	{
		ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
		return res;
	}

	PointCloudPtr PointCloudManager::Prepare(const std::string& filePath)
	{
		PointCloudPtr pMesh = std::static_pointer_cast<PointCloud>(CreateOrRetrieve(filePath));
		pMesh->VPrepare();
		return pMesh;
	}

	PointCloudPtr PointCloudManager::Load(const std::string& filePath)
	{
		PointCloudPtr pMesh = std::static_pointer_cast<PointCloud>(CreateOrRetrieve(filePath));
		pMesh->VLoad();
		return pMesh;
	}

	PointCloudPtr PointCloudManager::CreateManual(const std::string& name)
	{
		return Create(name);
	}

	Resource* PointCloudManager::VCreateImpl(const std::string& name, ResourceHandle handle)
	{
		// no use for createParams here
		return new PointCloud((ResourceManager*)this, name, handle);
	}
}
