#pragma once
#include <RenderDevice/BowRenderDeviceManager.h>

#include <string>

namespace bow
{
namespace examples
{

//! Render backend requested on the command line or in the environment.
/*!
    Looked up in this order:
      1. --backend <name> or --backend=<name>
      2. the LONGBOW_BACKEND environment variable
      3. the fallback passed in

    Accepted names are opengl / opengl3x / gl, directx12 / dx12 / d3d12, and
    vulkan / vk. An unrecognised name logs a warning and yields the fallback.

    \param argc     Argument count as handed to main.
    \param argv     Argument vector as handed to main.
    \param fallback Backend to use when nothing was requested.
    \return The selected backend.
*/
RenderDeviceAPI SelectBackend(int argc, char *argv[], RenderDeviceAPI fallback = RenderDeviceAPI::Vulkan);

//! Human-readable name of a backend, for logging.
const char *BackendName(RenderDeviceAPI api);

//! Number of positional arguments, i.e. everything SelectBackend did not consume.
int PositionalArgumentCount(int argc, char *argv[]);

//! Positional argument by index, counted after the program name.
/*!
    Lets an example take its own arguments without having to care where the
    backend option sits in argv.

    \param index Zero-based index among the positional arguments.
    \return The argument, or nullptr when there are fewer than index + 1.
*/
const char *PositionalArgument(int argc, char *argv[], int index);

//! Absolute path to a file below the repository's data/ directory.
/*!
    \param relative Path relative to data/, e.g. "Textures/test.png".
*/
std::string DataPath(const std::string &relative);

} // namespace examples
} // namespace bow
