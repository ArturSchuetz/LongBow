# Inventory

What exists across the LongBow lineages under `F:\Projects`, verified by reading the code rather
than counting files. Written 2026-08-05, on branch `modernize/cmake-merge`.

## Repositories

| Repo | Build | Last commit | Verdict |
|---|---|---|---|
| **LongBow** (this one, public on GitHub) | CMake (new) | active | The merge target |
| `long-bow-engine` (GitLab, private) | cmake-init, no longer configures under CMake 4 | 2026-03 | Source of the current module layout, API and Vulkan backend |
| `spherical-wavelets-for-globe-rendering` | CMake + presets | 2026-08 | 11 globe-rendering apps, published thesis |
| `Time-Of-Flight-Sensor-Simulation` | CMake + vcpkg, CUDA | 2024-03 | OpenGL + Dear ImGui, TOF simulation apps |
| `Masterthesis_Project`, `masterthesis` | CMake, CUDA | — | Same engine as TOF |
| `LongBow_old` | cmake-init | — | Superseded in every dimension |

## Render backends, as they actually are

| Backend | Files | State |
|---|---|---|
| **Vulkan** | 79 | Complete, including ray tracing. All nine examples render. Internally already organised as a thin API: `CommandBuffer`, `CommandPool`, `Fence`, `Semaphore`, `Pipeline`, `PipelineKey`, `RenderPass`, `Swapchain`, `PhysicalDevice`, `LogicalDevice`, `QueueFamily`, `DeviceMemory`, `RenderSurface`. |
| **OpenGL** | 59 | Complete against the current interfaces. Runs on a 4.5 core context. Buffers use direct state access; textures, framebuffers and vertex arrays are still bind-to-edit. |
| **DirectX 11** | 8 | Written from scratch here. Device, window, swap chain, clear, present, resize. No resource types at all. |
| **DirectX 12** | 4 | Stub taken from `long-bow-engine`; `VCreateWindow` itself is `LOG_FATAL`. A complete 41-file implementation against the *older* API lives in this repository's history at `2635b41`. |

### The DirectX 11 that is not there

`Time-Of-Flight-Sensor-Simulation` and `Masterthesis_Project` each carry 1836 lines under
`DirectX11RenderDevice`. It is not a DirectX 11 backend:

- it references `BowD3D11VertexBuffer.h`, `BowD3D11ShaderProgram.h`, `BowD3D11Texture2D.h`,
  `BowD3D11VertexArray.h`, framebuffer, uniform, index-buffer and write-pixel-buffer headers —
  **none of which exist in any repository**
- its render context calls `glDrawRangeElements`, `glDrawArrays` and
  `ImGui_ImplOpenGL3_RenderDrawData`: it is the OpenGL context with the class renamed
- it includes `d3dx11.h`, removed by Microsoft and absent from the current Windows SDK
- TOF's own `CMakeLists.txt` calls it "currently unreachable" and does not build it

### The DirectX 12 that is

In this repository's history, complete and against the older `IBowVertexArray` API:

```
ConstantBuffer  FragmentOutputs  GraphicsWindow  IndexBuffer  PixelBuffer  ReadPixelBuffer
RenderContext   RenderDevice     ShaderProgram   Texture2D    TextureSampler
TextureUnit(s)  TypeConverter    VertexArray     VertexBuffer VertexBufferAttributes
WritePixelBuffer + d3dx12.h
```

`d3dx12.h` is the still-supported helper header, unlike `d3dx11.h`.

## Engine modules

| Module | Present | Note |
|---|---|---|
| CoreSystems, Platform, Resources | yes | Math, logging, timing, geometry; image/mesh/point-cloud loaders |
| RenderDevice, InputDevice | yes | Interfaces plus the plugin-loading manager |
| NetworkDevice | **no** | Only in this repo's history. Its manager loaded `NetworkDevice.dll` while the only backend built `WinSockNetworkDevice.dll`, so it could never have worked |
| GameFoundation (actor/component) | **no** | Only in this repo's history; never in any solution |
| Scene | **no** | The old project existed but contained no source files |

## Examples

| Source | Examples |
|---|---|
| here / `long-bow-engine` | 00_TimerTest, 01_Input, 02_HelloWorld, 03_Triangle, 04_Cube, 05_Textures, 06_MeshRendering, 07_Framebuffer, 08_ComputeShader, 09_PathTracing |
| TOF / Masterthesis | 04_MeshRenderer, 05_SceneRenderer |
| spherical-wavelets | 11 globe-rendering apps (subdivision sphere, spherical ROAM, wavelet LOD, mouse picking, …) |

## Toolchain on this machine

| | |
|---|---|
| CMake | 4.2 |
| MSVC | 14.44 (VS 2022 17.14); **v140 is absent**, so the old solution cannot build here |
| Windows SDK | 10.0.26100 — DirectX 11 and 12 both available |
| Vulkan SDK | 1.4.341.1 |
| GPU | NVIDIA RTX 3090, OpenGL 4.5 core, D3D feature level 11.1 |

### Shader tooling, all from the Vulkan SDK

| Tool | Does |
|---|---|
| `dxc` 1.9 | HLSL → DXIL (DirectX 12), HLSL → SPIR-V (Vulkan) |
| `glslc`, `glslangValidator` | GLSL → SPIR-V |
| `spirv-cross` | SPIR-V → GLSL / HLSL / MSL |
| `d3dcompiler` (Windows SDK) | HLSL → DXBC (DirectX 11) |

Every translation the plan needs is therefore covered without adding a dependency.
