# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LongBow is a C++ game engine / rendering framework (`bow` namespace). Rendering, input and
networking backends are **runtime-loaded DLLs**; the engine core links only against abstract
interfaces.

> **Migration in progress:** this repo is the oldest of several LongBow variants under
> `F:\Projects` (see *Related repositories* below). It is Visual-Studio-only and has no CMake
> build. Everything documented here describes the current state, not the target state.

## Build

There is no CMake, no test suite and no lint configuration in this repo. The only build is the
Visual Studio solution `vs12/LongBow.sln`.

```bash
"C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" vs12/LongBow.sln -p:Configuration=Debug -p:Platform=x64 -m
```

Build a single project (target names use the solution's project names, `.` escaped as `_`):

```bash
"C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" vs12/LongBow.sln -t:OGL3xRenderDevice -p:Configuration=Debug -p:Platform=x64
```

- Configurations: `Debug` / `Release` × `Win32` / `x64` (the `ARM` entries in the `.sln` all map
  back to `Win32` and are not real).
- `PlatformToolset` is **v140** (VS2015) with `WindowsTargetPlatformVersion 10.0.16299.0` — VS2022
  alone is not enough, the v140 toolset and that SDK must be installed, or every `.vcxproj` needs
  retargeting.
- Output layout (all paths relative to repo root, `$(Platform)` = `Win32`|`x64`):
  - static libs → `lib/$(Platform)/`
  - DLLs **and** executables → `bin/$(Platform)/`
  - intermediates → `obj/$(Platform)/$(Configuration)/$(ProjectName)/`
  - `lib/` and `obj/` are gitignored; `bin/` is gitignored but `bin/Win32/glew32.dll`,
    `bin/x64/glew32.dll` and `bin/Data/` are force-added.
- **Debug binaries carry a `_d` suffix** (`TargetName = $(ProjectName)_d`). This is not cosmetic —
  the plugin loaders hardcode both spellings (see below).

Samples run with `LocalDebuggerWorkingDirectory = bin/$(Platform)/` and load assets as
`../Data/...`, i.e. relative to `bin/Data/`.

### Project dependencies

No `.vcxproj` uses `ProjectReference`. Build order comes from `ProjectSection(ProjectDependencies)`
in `LongBow.sln`, and linking comes from hand-maintained `AdditionalDependencies` lists naming
`.lib` files literally (`Resources_d.lib;CoreSystems_d.lib;RenderDevice_d.lib;…`). **Adding a
dependency means editing both places, in all four configurations.**

## Architecture

### Plugin/DLL model

`RenderDevice`, `InputDevice` and `NetworkDevice` are thin static libs holding only interfaces plus
a singleton manager. The manager `LoadLibraryExW`s a backend DLL by hardcoded filename and pulls a
single `extern "C"` factory out of it:

| Manager | Source | DLL loaded | Exported factory |
|---|---|---|---|
| `RenderDeviceManager::GetOrCreateDevice` | [BowRenderDeviceManager.cpp](src/Engine/RenderDevice/BowRenderDeviceManager.cpp) | `OGL3xRenderDevice[_d].dll`, `DirectX12RenderDevice[_d].dll`, `VulkanRenderDevice[_d].dll` | `CreateRenderDevice` |
| `InputDeviceManager` | [BowInputDeviceManager.cpp](src/Engine/InputDevice/BowInputDeviceManager.cpp) | `DirectInputDevice[_d].dll` | `CreateInputDevice` |
| `NetworkDeviceManager` | [BowNetworkDeviceManager.cpp](src/Engine/NetworkDevice/BowNetworkDeviceManager.cpp) | `NetworkDevice[_d].dll` | `CreateNetworkDevice` |

Every factory takes `EventLogger&`. Reason: singletons do not cross a DLL boundary on Windows, so
each DLL would otherwise build its own logger and its own log file. `DLLExport.cpp` in each backend
calls `EventLogger::SetInstance(logger)` before constructing the device — see
[DLLExport.cpp](src/Engine/RenderDeviceImplementations/OGL3xRenderDevice/DLLExport.cpp). Any new
cross-DLL singleton needs the same treatment.

Consequence: backend DLLs are never linked against, so a missing/renamed DLL is a runtime
`LOG_ERROR` and a `nullptr` device, not a link error.

### Layout

- `include/` — **all** public headers, flat, no subdirectories. `Bow*.h` = concrete types,
  `IBow*.h` = pure-virtual interfaces.
- `src/Engine/<Module>/` — implementations. `*RenderDeviceImplementations/<Backend>/` keeps its
  private headers next to its `.cpp`s.
- `vs12/<Group>/<Project>/` — one `.vcxproj` per module. Sample and demo **source code lives here
  too**, not under `src/`: `vs12/Samples/*/`, `vs12/GlobeRendering/*/`.
- `dependencies/glew`, `dependencies/glfw` — prebuilt Windows binaries, checked in.

### Conventions

- Namespace `bow`, everywhere.
- `V`-prefix marks a virtual method (`VCreateWindow`, `VDraw`, `VRelease`). Interfaces also carry an
  explicit `VRelease()` alongside the destructor.
- `XxxPtr` = `std::shared_ptr<Xxx>`, declared in the `*Predeclares.h` headers
  ([BowRendererPredeclares.h](include/BowRendererPredeclares.h),
  [BowCorePredeclares.h](include/BowCorePredeclares.h), …). Include the predeclare header, not the
  full type, in other headers.
- Umbrella headers per subsystem: `BowCore.h`, `BowRenderer.h`, `BowResources.h`, `BowInput.h`,
  `BowPlatform.h`, `BowMath.h`, `BowScene.h`. Samples include those.
- Logging via macros compiled out by level: `LOG_TRACE/INFO/WARNING/ERROR`, `LOG_ASSERT`,
  `LOG_FATAL` — [BowLogger.h](include/BowLogger.h). `LOG_LEVEL` defaults to `TRACE` in Debug,
  `INFO` in Release.
- Resource managers (`ImageManager`, `MeshManager`, `MaterialManager`, `PointCloudManager`) are
  singletons deriving from `ResourceManager`, which is a near-verbatim port of OGRE's design
  (handles + name maps, `VCreateImpl` hook) — [BowResourceManager.h](include/BowResourceManager.h).
- Sample shaders are embedded as Win32 resources (`.rc` + `resource.h`) and read back with
  `FindResource`/`LoadResource`, not loaded from disk.

### Backend status

| Backend | Windowing | State |
|---|---|---|
| `OGL3xRenderDevice` | GLFW | Complete — the reference implementation |
| `DirectX12RenderDevice` | raw Win32 `WNDCLASS`/`WindowProc` | Substantial; `VCreateVertexArray(mesh,…)` and `VCreateFramebuffer` still return `nullptr` ([BowD3D12RenderContext.cpp:154](src/Engine/RenderDeviceImplementations/DirectX12RenderDevice/BowD3D12RenderContext.cpp)) |
| `VulkanRenderDevice` | — | Stub; every factory returns `nullptr` ([BowVulkanRenderDevice.cpp:58](src/Engine/RenderDeviceImplementations/VulkanRenderDevice/BowVulkanRenderDevice.cpp)) |

Samples pick the API explicitly (`RenderDeviceAPI::OpenGL3x` / `::DirectX12`) and branch on it for
shader source and attribute names (`in_Position` vs `POSITION0`) — see
[Triangle.cpp:39](vs12/Samples/02_Triangle/Triangle.cpp).

## Known broken/dead spots

- `NetworkDeviceManager` loads `NetworkDevice[_d].dll`, but the only network backend project builds
  `WinSockNetworkDevice[_d].dll`. Networking cannot load as-is.
- `vs12/Engine/GameFoundation/GameFoundation.vcxproj` exists and `src/Engine/GameFoundation/` has
  sources (`BowActor`, `BowActorComponent`, `BowGameLogic`, …), but the project is **not in the
  solution** and is never built.
- `vs12/Engine/Scene/Scene.vcxproj` *is* in the solution but contains zero source files.
  `include/BowScene.h` includes `IBowRenderStrategy.h` and `BowLegacyVertexLitRenderer.h`, neither
  of which exists — including it does not compile.
- `vs12/Samples/Quaternions/` holds shader/resource files only and is not in the solution
  (superseded by `07_Quaternions`).
- `Doxyfile` hardcodes absolute paths from a different machine
  (`C:/Users/Greg/Documents/Visual Studio 2017/Projects/LongBow/...` for `INPUT` and
  `OUTPUT_DIRECTORY`), so `doxygen Doxyfile` documents nothing here despite what the README claims.
- `math::Sqrt` in [BowMath.h](include/BowMath.h) is the Quake fast **inverse** square root, returned
  as if it were `Sqrt`, and the `double` overload type-puns through `long`.
- Doc comments are largely German (`\~german`), a few are bilingual.

## Related repositories

Other LongBow lineages live beside this one under `F:\Projects` and are ahead of it in different
dimensions. Consult them before reimplementing anything:

| Repo | State | Strongest part |
|---|---|---|
| `long-bow-engine` | C++17, CMake, cross-platform, GTest, clang-format/tidy, Optick | Vulkan incl. ray tracing (79 files), examples 00–09; OpenGL disabled, DX12 near-empty |
| `spherical-wavelets-for-globe-rendering` | CMake + presets, published README/thesis | Engine re-split into `Core`/`Renderer`/`RenderSystems`/…, 11 globe-rendering apps |
| `Time-Of-Flight-Sensor-Simulation`, `Masterthesis_Project`, `masterthesis` | CMake, CUDA | Largest OpenGL backend (67 files), DirectX **11** backend (21 files) |
| `LongBow_old` | CMake (cmake-init template) | DX11/DX12/Vulkan skeletons |
| *this repo* | VS2015 solution only | **Largest DirectX 12 backend (41 files)** |
