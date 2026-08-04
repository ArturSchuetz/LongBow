#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <iostream>

int main(int /*argc*/, char * /*argv[]*/)
{
    FN("main");

    OPTICK_APP("Hello World Sample");

    LOG_TRACE("Hello World!");

    // Creating Render Device
    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(bow::RenderDeviceAPI::Vulkan);
    if (device == nullptr)
    {
        std::cout << "Could not create device!" << std::endl;
        return -1;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(800, 600, "HelloWorld", bow::WindowType::Windowed);
    if (window == nullptr)
    {
        std::cout << "Could not create window!" << std::endl;
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // Input

    bow::KeyboardPtr keyboard = bow::InputDeviceManager::GetInstance().CreateKeyboardObject(window);
    if (keyboard == nullptr)
    {
        std::cout << "Could not create keyboard object!" << std::endl;
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // Gameloop
    auto renderSurface = window->VGetContext();

    while (!window->VShouldClose())
    {
        OPTICK_FRAME("MainThread");

        keyboard->VUpdate();
        if (keyboard->VIsPressed(bow::Key::K_ESCAPE))
            break;

        renderSurface->VSwapBuffers();
        LOG_UPDATE();
    }

    OPTICK_SHUTDOWN();

    return 0;
}