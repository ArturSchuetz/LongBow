# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LongBow is a C++17 rendering framework (`bow` namespace). Render, input and network backends are
**runtime-loaded shared libraries**; the engine core links only against abstract interfaces.

> **Migration in progress** on branch `modernize/cmake-merge`. The Visual-Studio-only tree
> (`src/`, `include/`, `vs12/`) is gone; `source/` replaces it. See [TODO.md](TODO.md) for the
> staged plan and the current test matrix.

## Build

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

Executables and backend plugins both land in `build/bin/`. That is required, not cosmetic: the
managers open a backend by bare filename, so a plugin anywhere else is not found at runtime.
Debug binaries carry a `d` suffix (`VulkanRenderDeviced.dll`), which the loaders hardcode.

### Options

| Option | Default |
|---|---|
| `LONGBOW_BUILD_OPENGL` / `_DIRECTX11` / `_DIRECTX12` / `_VULKAN` | auto-detected |
| `LONGBOW_BUILD_EXAMPLES` | ON when top-level |
| `LONGBOW_BUILD_TESTS`, `LONGBOW_BUILD_DOCS`, `LONGBOW_ENABLE_PROFILER` | OFF |

Backend defaults come from `longbow_backend_available()` in the root `CMakeLists.txt`: the
platform must support it *and* its sources must exist. DirectX 11 has not been ported yet, so it
is off everywhere. Turning an unavailable backend on is a hard error, not a silent skip.

### Dependencies

Nothing is vendored except LoadPNG. glslang, shaderc, SPIRV-Tools and SPIRV-Cross come from the
**Vulkan SDK** (`VULKAN_SDK`); GLFW, GLEW and Optick are fetched at configure time. An installed
GLEW (vcpkg, system) is preferred over the fetch.

`find_package` in `source/ThirdParty/CMakeLists.txt` must pass `GLOBAL` — imported targets are
otherwise scoped to the directory that found them, and the backends consuming them are siblings.

## Architecture

### Module layout

```
source/<Module>/include/<Module>/*.h   public headers, included as <Module/Foo.h>
source/<Module>/source/*.cpp           implementation plus private headers
```

`cmake/LongBowModule.cmake` derives everything else from the module name. A module's CMakeLists is
its name plus its dependencies; `longbow_add_plugin` is the same thing forced to SHARED.
`generate_export_header` produces `<Module>/<Module>_api.h`, defining `<MODULE>_API`.

Dependency order: `CoreSystems` → `Platform` → `Resources` → `RenderDevice` / `InputDevice` →
backends → `Examples`.

### Plugin model

`RenderDeviceManager`, `InputDeviceManager` and `NetworkDeviceManager` `LoadLibrary` a backend by
hardcoded filename and pull one `extern "C"` factory out of it (`CreateRenderDevice`,
`CreateInputDevice`, `CreateNetworkDevice`).

Every factory takes `EventLogger&`. Singletons do not cross a shared-library boundary on Windows,
so each plugin would otherwise build its own logger and its own log file; `DLLExport.cpp` calls
`EventLogger::SetInstance(logger)` before constructing the device. **Any new cross-plugin singleton
needs the same treatment.** A missing or renamed plugin is a runtime `LOG_ERROR` and a `nullptr`
device, never a link error.

The loader is still Windows-only despite the framework otherwise being portable.

### Conventions

- Namespace `bow`, everywhere.
- `V`-prefix marks a virtual method (`VCreateWindow`, `VDraw`, `VRelease`). Interfaces carry an
  explicit `VRelease()` alongside the destructor.
- `XxxPtr` = `std::shared_ptr<Xxx>`, declared in the `*Predeclares.h` headers. Include the
  predeclare header rather than the full type in other headers.
- Logging macros compile out by level: `LOG_TRACE/INFO/WARNING/ERROR`, `LOG_ASSERT`, `LOG_FATAL`.
  `FN("Scope::Name")` at the top of a function pushes a call-stack frame.
- **`LOG_FATAL` opens a modal message box and trips an assert.** Set `LONGBOW_NO_ERROR_DIALOG` for
  unattended runs, or the process blocks until someone clicks. It does not terminate, so code
  after it keeps running and typically segfaults on the `nullptr` it just logged about.

### Examples

Examples select their backend at runtime through `bow::examples::SelectBackend` — `--backend
opengl|directx12|vulkan` or `LONGBOW_BACKEND`, defaulting to Vulkan. Use
`bow::examples::PositionalArgument` for an example's own arguments so the option cannot collide,
and `bow::examples::DataPath` for anything under `data/`.

### Backend status

Measured by running every example against every backend; the table lives in [TODO.md](TODO.md).

| Backend | Windowing | State |
|---|---|---|
| Vulkan | GLFW | Complete, including ray tracing. All nine graphics examples render. |
| OpenGL 3.x | GLFW + GLEW | Initialises and renders `02_HelloWorld`; the rest stop at the five unimplemented shader-resource-binding functions in `BowOGL3xShaderProgram.cpp`. |
| DirectX 12 | — | Four-file stub; `VCreateWindow` is `LOG_FATAL`. The real 41-file implementation is in this repo's history, against the older `IBowVertexArray` API. |
| DirectX 11 | — | Not ported. Source is in `Time-Of-Flight-Sensor-Simulation`. |

## Related repositories

Other LongBow lineages live beside this one under `F:\Projects`. Consult them before
reimplementing anything:

| Repo | Strongest part |
|---|---|
| `long-bow-engine` | Origin of this tree. Vulkan incl. ray tracing, examples 00–09 |
| `Time-Of-Flight-Sensor-Simulation`, `Masterthesis_Project`, `masterthesis` | Largest OpenGL backend (67 files, incl. Dear ImGui), DirectX **11** backend (21 files) |
| `spherical-wavelets-for-globe-rendering` | 11 globe-rendering apps, published README/thesis |
| `LongBow_old` | DX11/DX12/Vulkan skeletons |

## Known broken/dead spots

- `NetworkDevice` and `GameFoundation` exist only in this repo's history; `long-bow-engine` never
  had them. The old `NetworkDeviceManager` loaded `NetworkDevice.dll` while the only backend built
  `WinSockNetworkDevice.dll`, so it could never have loaded.
- `Doxyfile` hardcodes absolute paths from another machine (`C:/Users/Greg/...`).
- Doc comments are largely German; a handful of source files are Latin-1 rather than UTF-8 and
  produce C4828 warnings.
