#include "OGLRenderDevice.h"

namespace Bow{
	
	extern "C" __declspec(dllexport) bool CreateRenderDevice(IRenderDevice **pDevice, HINSTANCE hInstance)
	{
		FN("CreateRenderDevice()");
		if(!*pDevice)
		{
			*pDevice = new OGLRenderDevice(hInstance);
			return true;
		}
		LOG_ASSERT(false, "Someting went wrong O.o.");
		return false;
	}
}