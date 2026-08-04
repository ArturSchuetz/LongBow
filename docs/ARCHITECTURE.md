# Architecture

LongBow exposes **two rendering interfaces**, not one. This document says why, what belongs in
each, and how the two relate.

## Why two

The interface as it stands is a hybrid. `IRenderDevice` carries `VClear` and a `VDraw` that takes
a `RenderState` by value — the vocabulary of an immediate-mode API — next to acceleration
structures, shader resource bindings, push constants, `VBeginFrame`/`VEndFrame` and `VTraceRays`,
which are the vocabulary of an explicit one.

Neither side is served by that:

- **OpenGL and DirectX 11** have to pretend they have descriptor sets. The OpenGL backend
  implements `IShaderResourceBindings` as a record that it replays before every draw, because
  OpenGL binds against the current program and has nothing to prepare in advance. Push constants
  are rewritten into uniform blocks. That is emulation, and it costs both performance and clarity.
- **Vulkan and DirectX 12** have to hide what they are good at. The Vulkan backend already
  contains command buffers, command pools, fences, semaphores, pipeline objects keyed by state,
  render passes and a swapchain — and then presents all of it through `VClear`/`VDraw`/
  `VSwapBuffers`. Every frame goes through a facade that throws away the explicitness the API was
  chosen for.

So: one interface per family, each shaped like the APIs behind it.

```
                     Applications / Examples
                    /                        \
        Classic Renderer                  Thin Renderer
      (immediate, stateful)             (explicit, recorded)
       /            \                     /            \
   OpenGL 4.5   DirectX 11           Vulkan 1.3    DirectX 12
                    \                    /
                     Shared foundation
        CoreSystems · Platform · Resources · shader pipeline
```

Both tiers sit on the same `CoreSystems`, `Platform` and `Resources`, use the same window and
input abstractions, and are loaded the same way — one plugin per backend, opened by name at
runtime.

## Classic Renderer

**Backends:** OpenGL 4.5, DirectX 11
**Model:** an immediate context owns the pipeline state; resources are addressed by the name they
carry in the shader; the driver decides when work is submitted.

Keeps: windows and swap chains, vertex/index/uniform/storage buffers, textures and samplers,
shader programs, vertex attribute bindings, framebuffers, render states, compute shaders.

Loses, relative to the interface as it stands today:

| Removed | Why | Replaced by |
|---|---|---|
| Acceleration structures, `VTraceRays`, ray-tracing programs | Neither API has them | Thin renderer |
| `VBeginFrame` / `VEndFrame` | Nothing to record between them | — |
| Push constants | Emulated as uniform blocks anyway | Uniform buffers, set by name |
| `IShaderResourceBindings` as a descriptor set | Emulated as a replay list | Binding by name on the context |

The point is not to make the classic tier weaker. It is to stop it from carrying vocabulary its
backends cannot honour, so that what it does expose is exactly what it can do well.

## Thin Renderer

**Backends:** Vulkan 1.3, DirectX 12
**Model:** the application records command buffers, owns synchronisation, and builds pipeline
state up front.

The object set is chosen so that each concept maps onto both APIs without emulation:

| Thin object | Vulkan | DirectX 12 |
|---|---|---|
| `IThinDevice` | `VkDevice` + `VkPhysicalDevice` | `ID3D12Device` + adapter |
| `IThinQueue` | `VkQueue` | `ID3D12CommandQueue` |
| `IThinCommandPool` / `IThinCommandList` | `VkCommandPool` / `VkCommandBuffer` | `ID3D12CommandAllocator` / `ID3D12GraphicsCommandList` |
| `IThinSwapchain` | `VkSwapchainKHR` | `IDXGISwapChain3` |
| `IThinFence` | timeline semaphore | `ID3D12Fence` |
| `IThinBuffer` / `IThinTexture` | `VkBuffer` / `VkImage` + memory | `ID3D12Resource` |
| `IThinPipeline` | `VkPipeline` | `ID3D12PipelineState` |
| `IThinPipelineLayout` | descriptor set layouts + push constants | root signature |
| `IThinDescriptorSet` / `IThinDescriptorPool` | `VkDescriptorSet` / `VkDescriptorPool` | descriptor heap range |
| `IThinAccelerationStructure` | `VkAccelerationStructureKHR` | `ID3D12Resource` (DXR) |

Four decisions make that table work:

1. **Dynamic rendering, no render-pass objects.** Vulkan 1.3 promoted `VK_KHR_dynamic_rendering`
   to core, which is how DirectX 12 has always worked (`OMSetRenderTargets`). Modelling render
   passes would mean inventing them for DirectX 12; not modelling them costs Vulkan nothing.
2. **Timeline semaphores only.** A Vulkan timeline semaphore and a DirectX 12 fence are the same
   object: a monotonically increasing counter waited on by value. Binary semaphores exist in
   Vulkan only for swapchain acquire, which the swapchain object hides.
3. **Explicit resource states.** A single `ResourceState` enum maps to `VkImageLayout` plus access
   flags on one side and `D3D12_RESOURCE_STATES` on the other. Barriers are part of the interface,
   not something the backend guesses at.
4. **Descriptor sets are tables, pipeline layouts are ordered lists of tables plus root/push
   constants.** That is literally both models.

## Shaders

Authoring language is chosen per tier, and everything is translated at build time so that neither
a shipped example nor a consuming application needs a compiler at runtime.

| Tier | Backend | Authored in | Path |
|---|---|---|---|
| Thin | Vulkan | HLSL | `dxc -spirv` → SPIR-V |
| Thin | DirectX 12 | HLSL | `dxc` → DXIL |
| Classic | OpenGL 4.5 | GLSL | fed to the driver, or `glslc` → SPIR-V for `GL_ARB_gl_spirv` |
| Classic | DirectX 11 | HLSL | `d3dcompiler` → DXBC |

HLSL is the authoring language for the thin tier because `dxc` targets both DXIL and SPIR-V from
one source, which is what removes the need for two shader sets.

For the classic tier the examples exist twice on purpose — a GLSL set and an HLSL set — so that
both paths are exercised and documented rather than one being translated behind the reader's
back. Where a single source is wanted, `glslc` → `spirv-cross --hlsl` covers GLSL → HLSL, and
that path is available but not the default.

All four tools ship with the Vulkan SDK and the Windows SDK; nothing new is vendored.

## What stays shared

`CoreSystems` (math, logging, timing, geometry), `Platform` (file I/O), `Resources` (images,
meshes, materials, point clouds), `InputDevice`, and the window abstraction are used by both
tiers unchanged. A mesh loaded through `MeshManager` feeds either renderer.

## Plugin model

Unchanged, and applied to both tiers: each backend builds to a shared library that the matching
manager opens by name and resolves a single `extern "C"` factory out of. The factory takes the
`EventLogger` by reference because singletons do not cross a shared-library boundary on Windows.

Classic backends export `CreateRenderDevice`; thin backends export `CreateThinDevice`. A build
may contain any subset.
