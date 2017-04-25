#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow {
	namespace Core {

		class MemoryManager
		{
		protected:
			MemoryManager();

		public:
			~MemoryManager();

			static MemoryManager& GetInstance();

			void Release();

		private:
			MemoryManager(const MemoryManager&){}; // You shall not copy
			MemoryManager& operator=(const MemoryManager&) { return *this; }
		};

	}
}