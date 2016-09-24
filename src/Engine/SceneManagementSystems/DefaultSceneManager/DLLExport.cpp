#include "BowLogger.h"
#include "BowDefaultSceneManager.h"

namespace Bow{
	namespace Scene{

		using namespace Core;

		extern "C" __declspec(dllexport) ISceneManager* CreateRenderDevice(EventLogger& logger)
		{
			// set the new instance of the logger to prevent the creation of a new one inside this dll
			EventLogger::SetInstance(logger);
			DefaultSceneManager* sceneManager = new DefaultSceneManager();

			if (!sceneManager->Init())
			{
				delete sceneManager;
				return nullptr;
			}
			return sceneManager;
		}

	}
}