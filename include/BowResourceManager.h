#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow
{
	namespace Core
	{
		class ResourceManager
		{
		protected:
			ResourceManager();

		public:
			~ResourceManager();

			static ResourceManager& GetInstance();

			void Release();

			MeshPtr LoadMesh(const std::string& path);

		private:
			ResourceManager(const ResourceManager&){}; // You shall not copy
			ResourceManager& operator=(const ResourceManager&) { return *this; }
		};

	}
}