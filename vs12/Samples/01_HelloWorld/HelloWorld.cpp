#include "BowRenderer.h"
#include "BowInput.h"

#include <cstdint>
#include <windows.h>

using namespace bow;

int main()
{
	// Creating Render Device
	RenderDevicePtr deviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(RenderDeviceAPI::DirectX12);
	if (deviceOGL == nullptr)
	{
		std::cout << "Could not create device!" << std::endl;
		return -1;
	}

	// Creating Window
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "HelloWorld", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		std::cout << "Could not create window!" << std::endl;
		return -1;
	}

	// Change ClearColor
	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Input

	KeyboardPtr keyboard = InputDeviceManager::GetInstance().CreateKeyboardObject(windowOGL);
	if (keyboard == nullptr)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// Gameloop
	auto contextOGL = windowOGL->VGetContext();

	while (!windowOGL->VShouldClose())
	{
		keyboard->VUpdate();
		if (keyboard->VIsPressed(Key::K_ESCAPE))
			break;

		// Clear Backbuffer to our ClearState
		contextOGL->VClear(clearState);

		contextOGL->VSwapBuffers();

		windowOGL->VPollWindowEvents();
	}
	return 0;
}