#include "BowLogger.h"
#include "BowOGLRenderDevice.h"

namespace Bow{
	namespace Renderer{

		using namespace Core;

		extern "C" __declspec(dllexport) IRenderDevice* CreateRenderDevice(EventLogger& logger)
		{
			// set the new instance of the logger to prevent the creation of a new one inside this dll
			EventLogger::SetInstance(logger);
			return new OGLRenderDevice();
		}

	}
}