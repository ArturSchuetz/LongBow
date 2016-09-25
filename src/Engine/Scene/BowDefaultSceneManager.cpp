#include "BowDefaultSceneManager.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

namespace Bow {
	namespace Scene {

		using namespace Core;

		void error_callback(int error, const char* description)
		{
			LOG_ERROR(description);
		}

		DefaultSceneManager::DefaultSceneManager(void)
		{
			
		}

		DefaultSceneManager::~DefaultSceneManager(void)
		{
			VRelease();
		}

		bool DefaultSceneManager::Init(void)
		{
			return true;
		}

		void DefaultSceneManager::VRelease(void)
		{

		}

		void DefaultSceneManager::VCreateMesh(void)
		{

		}

		void DefaultSceneManager::VDrawAll(void)
		{

		}
	}
}