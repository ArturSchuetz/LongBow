#pragma once
#include "BowPrerequisites.h"

#include "IBowSceneManager.h"

namespace Bow {
	namespace Scene {

		class DefaultSceneManager : public ISceneManager
		{
		public:
			DefaultSceneManager(void);
			~DefaultSceneManager(void);

			bool Init(void);
			void VRelease(void);

			void VCreateMesh(void);
			void VDrawAll(void);

		private:		
			//you shall not copy
			DefaultSceneManager(DefaultSceneManager&){}
			DefaultSceneManager& operator=(const DefaultSceneManager&) { return *this; }
		};

		typedef std::shared_ptr<DefaultSceneManager> OGLRenderDevicePtr;
		typedef std::unordered_map<int, DefaultSceneManager> OGLRenderDeviceMap;

	}
}