#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>

using namespace bow;



int main()
{
	// Creating Render Device
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(RenderDeviceAPI::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return -1;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "HelloWorld", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return -1;
	}

	// Change ClearColor
	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Gameloop
	auto ContextOGL = WindowOGL->VGetContext();

	while (!WindowOGL->VShouldClose())
	{
		// Clear Backbuffer to our ClearState
		ContextOGL->VClear(clearState);

		ContextOGL->VSwapBuffers();

		WindowOGL->VPollWindowEvents();
	}
	return 0;
}