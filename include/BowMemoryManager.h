/**
 * @file BowMemoryManager.h
 * @brief Declarations for BowMemoryManager.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace bow {

	class MemoryManager
	{
	protected:
		MemoryManager();

	public:
		~MemoryManager();

		static MemoryManager& GetInstance();

		void Release();

	private:
		MemoryManager(const MemoryManager&){}; // You shall not direct
		MemoryManager& operator=(const MemoryManager&) { return *this; }
	};

}
