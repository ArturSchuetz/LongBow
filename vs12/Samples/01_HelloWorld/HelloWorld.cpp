#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>

using namespace Bow;
using namespace Core;
using namespace Renderer;

int main()
{
	// Creating Render Device
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "HelloWorld", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
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