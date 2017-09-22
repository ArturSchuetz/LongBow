#include "BowCore.h"
#include "BowMemoryManager.h"

namespace Bow {
	namespace Core {

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
			static MemoryManager instance;
			return instance;
		}
	}
}
