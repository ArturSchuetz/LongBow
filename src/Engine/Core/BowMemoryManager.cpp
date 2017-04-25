#include "BowCore.h"
#include "BowMemoryManager.h"

namespace Bow {
	namespace Core {

		static std::shared_ptr<MemoryManager> Instance;

		MemoryManager::MemoryManager()
		{

		}

		MemoryManager::~MemoryManager()
		{
			Release();
		}

		void MemoryManager::Release()
		{

		}

		MemoryManager& MemoryManager::GetInstance()
		{
			if (Instance.get() == nullptr)
			{
				Instance = std::shared_ptr<MemoryManager>(new MemoryManager());
			}
			return *Instance.get();
		}
	}
}