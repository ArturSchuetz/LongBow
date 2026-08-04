#include <ExampleSupport/ExampleSupport.h>

#include <CoreSystems/BowLogger.h>

#include <cstdlib>
#include <cstring>
#include <string>

#ifndef LONGBOW_EXAMPLE_DATA_DIR
#define LONGBOW_EXAMPLE_DATA_DIR "data"
#endif

namespace bow
{
namespace examples
{
namespace
{

bool IsBackendOption(const char *argument)
{
    return std::strcmp(argument, "--backend") == 0 || std::strncmp(argument, "--backend=", 10) == 0;
}

bool ParseBackendName(const std::string &name, RenderDeviceAPI &api)
{
    std::string lowered;
    lowered.reserve(name.size());
    for (char c : name)
    {
        lowered += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    if (lowered == "opengl" || lowered == "opengl3x" || lowered == "gl")
    {
        api = RenderDeviceAPI::OpenGL3x;
        return true;
    }
    if (lowered == "directx11" || lowered == "dx11" || lowered == "d3d11")
    {
        api = RenderDeviceAPI::DirectX11;
        return true;
    }
    if (lowered == "directx12" || lowered == "dx12" || lowered == "d3d12")
    {
        api = RenderDeviceAPI::DirectX12;
        return true;
    }
    if (lowered == "vulkan" || lowered == "vk")
    {
        api = RenderDeviceAPI::Vulkan;
        return true;
    }
    return false;
}

} // namespace

RenderDeviceAPI SelectBackend(int argc, char *argv[], RenderDeviceAPI fallback)
{
    std::string requested;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strncmp(argv[i], "--backend=", 10) == 0)
        {
            requested = argv[i] + 10;
            break;
        }
        if (std::strcmp(argv[i], "--backend") == 0 && i + 1 < argc)
        {
            requested = argv[i + 1];
            break;
        }
    }

    if (requested.empty())
    {
        const char *fromEnvironment = std::getenv("LONGBOW_BACKEND");
        if (fromEnvironment != nullptr)
        {
            requested = fromEnvironment;
        }
    }

    if (requested.empty())
    {
        return fallback;
    }

    RenderDeviceAPI api = fallback;
    if (!ParseBackendName(requested, api))
    {
        LOG_WARNING("Unknown backend '%s', falling back to %s. Expected one of: opengl, directx11, directx12, vulkan.", requested.c_str(), BackendName(fallback));
        return fallback;
    }

    return api;
}

const char *BackendName(RenderDeviceAPI api)
{
    switch (api)
    {
    case RenderDeviceAPI::OpenGL3x:
        return "OpenGL 3.x";
    case RenderDeviceAPI::DirectX11:
        return "DirectX 11";
    case RenderDeviceAPI::DirectX12:
        return "DirectX 12";
    case RenderDeviceAPI::Vulkan:
        return "Vulkan";
    }
    return "unknown";
}

int PositionalArgumentCount(int argc, char *argv[])
{
    int count = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (IsBackendOption(argv[i]))
        {
            // Skip the value too when it was given as a separate token.
            if (std::strcmp(argv[i], "--backend") == 0)
            {
                ++i;
            }
            continue;
        }
        ++count;
    }
    return count;
}

const char *PositionalArgument(int argc, char *argv[], int index)
{
    int seen = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (IsBackendOption(argv[i]))
        {
            if (std::strcmp(argv[i], "--backend") == 0)
            {
                ++i;
            }
            continue;
        }
        if (seen == index)
        {
            return argv[i];
        }
        ++seen;
    }
    return nullptr;
}

std::string DataPath(const std::string &relative)
{
    return std::string(LONGBOW_EXAMPLE_DATA_DIR) + "/" + relative;
}

} // namespace examples
} // namespace bow
