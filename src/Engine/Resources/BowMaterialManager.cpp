#include "BowResources.h"
#include "BowMaterialManager.h"

namespace bow {
	
	static std::shared_ptr<MaterialManager> Instance;

	MaterialManager::MaterialManager()
	{
		m_resourceType = "Material";
	}

	MaterialManager::~MaterialManager()
	{
	}

	MaterialManager& MaterialManager::GetInstance()
	{
		if (Instance.get() == nullptr)
		{
			Instance = std::shared_ptr<MaterialManager>(new MaterialManager());
			Instance->Initialize();
		}
		return *Instance.get();
	}

	void MaterialManager::Initialize(void)
	{

	}

	MaterialCollectionPtr MaterialManager::Create(const std::string& name)
	{
		return std::static_pointer_cast<MaterialCollection>(CreateResource(name));
	}

	ResourcePtr MaterialManager::CreateOrRetrieve(const std::string& filePath)
	{
		ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
		return res;
	}

	MaterialCollectionPtr MaterialManager::Prepare(const std::string& filePath)
	{
		MaterialCollectionPtr pMaterial = std::static_pointer_cast<MaterialCollection>(CreateOrRetrieve(filePath));
		pMaterial->VPrepare();
		return pMaterial;
	}

	MaterialCollectionPtr MaterialManager::Load(const std::string& filePath)
	{
		MaterialCollectionPtr pMaterial = std::static_pointer_cast<MaterialCollection>(CreateOrRetrieve(filePath));
		pMaterial->VLoad();
		return pMaterial;
	}

	MaterialCollectionPtr MaterialManager::CreateManual(const std::string& name)
	{
		return Create(name);
	}

	Resource* MaterialManager::VCreateImpl(const std::string& name, ResourceHandle handle)
	{
		// no use for createParams here
		return new MaterialCollection((ResourceManager*)this, name, handle);
	}
}
