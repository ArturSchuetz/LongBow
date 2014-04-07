#include "BowRenderDeviceManager.h"

#include "BowClearState.h"

#include "IBowGraphicsWindow.h"
#include "IBowRenderDevice.h"
#include "IBowRenderContext.h"

#include <cstdint>
#include <windows.h>

int main()
{
	// Creating Render Device
	Bow::Renderer::RenderDevicePtr DeviceOGL = Bow::Renderer::RenderDeviceManager::GetInstance().GetOrCreateDevice(Bow::Renderer::API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	Bow::Renderer::GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "HelloWorld", Bow::Renderer::WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	// Change ClearColor
	Bow::Renderer::ClearState clearState;
	float cornflowerBlue[] = { 0.392f, 0.584f, 0.929f, 1.0f };
	memcpy(&clearState.Color, &cornflowerBlue, sizeof(float)* 4);

	///////////////////////////////////////////////////////////////////
	// Gameloop
	auto ContextOGL = WindowOGL->VGetContext();

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Clear Backbuffer to our ClearState
			ContextOGL->VClear(clearState);

			ContextOGL->VSwapBuffers();
		}
	}
	return (int)msg.wParam;
}