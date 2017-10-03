#include "BowResources.h"
#include "BowMeshManager.h"

namespace Bow {
	namespace Core {

		static std::shared_ptr<MeshManager> Instance;

		MeshManager::MeshManager()
		{
			m_resourceType = "Mesh";
		}

		MeshManager::~MeshManager()
		{
		}

		MeshManager& MeshManager::GetInstance()
		{
			if (Instance.get() == nullptr)
			{
				Instance = std::shared_ptr<MeshManager>(new MeshManager());
				Instance->Init();
			}
			return *Instance.get();
		}

		void MeshManager::Init(void)
		{

		}

		MeshPtr MeshManager::Create(const std::string& name)
		{
			return std::static_pointer_cast<Mesh>(CreateResource(name));
		}

		ResourcePtr MeshManager::CreateOrRetrieve(const std::string& filePath)
		{
			ResourcePtr res = ResourceManager::CreateOrRetrieve(filePath);
			return res;
		}

		MeshPtr MeshManager::Prepare(const std::string& filePath)
		{
			MeshPtr pMesh = std::static_pointer_cast<Mesh>(CreateOrRetrieve(filePath));
			pMesh->VPrepare();
			return pMesh;
		}

		MeshPtr MeshManager::Load(const std::string& filePath)
		{
			MeshPtr pMesh = std::static_pointer_cast<Mesh>(CreateOrRetrieve(filePath));
			pMesh->VLoad();
			return pMesh;
		}

		MeshPtr MeshManager::CreateManual(const std::string& name)
		{
			return Create(name);
		}

		Resource* MeshManager::VCreateImpl(const std::string& name, ResourceHandle handle)
		{
			// no use for createParams here
			return new Mesh((ResourceManager*)this, name, handle);
		}
	}
}