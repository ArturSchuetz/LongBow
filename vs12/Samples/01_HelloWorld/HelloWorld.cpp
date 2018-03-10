#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>

using namespace bow;



int main()
{
	// Creating Render Device
	RenderDevicePtr deviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(RenderDeviceAPI::OpenGL3x);
	if (deviceOGL == nullptr)
	{
		return -1;
	}

	// Creating Window
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "HelloWorld", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return -1;
	}

	// Change ClearColor
	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Gameloop
	auto contextOGL = windowOGL->VGetContext();

	while (!windowOGL->VShouldClose())
	{
		// Clear Backbuffer to our ClearState
		contextOGL->VClear(clearState);

		contextOGL->VSwapBuffers();

		windowOGL->VPollWindowEvents();
	}
	return 0;
}