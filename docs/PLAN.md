# Action plan

Ordered by dependency. Each stage ends in something runnable, so progress is visible rather than
inferred. See [ARCHITECTURE.md](ARCHITECTURE.md) for the design this implements and
[INVENTORY.md](INVENTORY.md) for what the starting material actually is.

Effort is rough: **S** under a day, **M** a few days, **L** a week or more.

---

## Stage 1 — Split the interface in two

Nothing else can be built cleanly until the classic and thin tiers are separate.

| # | Task | Effort |
|---|---|---|
| 1.1 | New module `ThinRenderDevice` alongside `RenderDevice`, same layout and plugin conventions | S |
| 1.2 | Move ray tracing out of `RenderDevice`: `IBowBottomLevel-`/`ITopLevelAccelerationStructure`, `IBowRayTracingShaderProgram`, `VTraceRays`, the two `VCreate*AccelerationStructure` methods | S |
| 1.3 | Drop `VBeginFrame`/`VEndFrame` and `VSetPushConstants` from the classic interface; uniform buffers replace push constants | S |
| 1.4 | Replace `IShaderResourceBindings` in the classic tier with binding by name on the context (`VSetTexture`, `VSetUniformBuffer`, `VSetStorageBuffer`) | M |
| 1.5 | Follow the changes through the OpenGL backend; it loses the replay list it only had to emulate descriptor sets | M |
| 1.6 | `ThinRenderDeviceManager` mirroring `RenderDeviceManager`, resolving `CreateThinDevice` | S |

**Done when:** OpenGL still passes the example matrix, and the classic interface no longer
mentions anything the classic backends cannot do.

---

## Stage 2 — OpenGL 4.5 throughout

Buffers already use direct state access. The rest does not.

| # | Task | Effort |
|---|---|---|
| 2.1 | Textures: `glCreateTextures`, `glTextureStorage2D` (immutable storage), `glTextureSubImage2D`, `glTextureParameteri` — replaces 2 `glGenTextures`, 4 `glTexImage2D`, 8 `glTexParameteri` | M |
| 2.2 | Framebuffers: `glCreateFramebuffers`, `glNamedFramebufferTexture`, `glNamedFramebufferDrawBuffers`, `glCheckNamedFramebufferStatus` | S |
| 2.3 | Vertex arrays: `glCreateVertexArrays`, `glVertexArrayVertexBuffer`, `glVertexArrayAttribFormat`/`AttribBinding` — separates format from buffer, which is what DirectX 11 input layouts do too | M |
| 2.4 | Texture binding: `glBindTextureUnit` instead of `glActiveTexture` + `glBindTexture` | S |
| 2.5 | Retire the remaining 20 `glBindBuffer` calls where DSA has an equivalent | S |
| 2.6 | Raise the context floor to 4.5 and delete the 3.3 fallback, or keep the fallback and gate DSA behind the flag already present | S |
| 2.7 | Rename the module `OpenGL3xRenderDevice` → `OpenGLRenderDevice`, and `OGL3x*` → `OGL*` | S |
| 2.8 | Optional 4.5 wins: `glClipControl` for a zero-to-one depth range matching DirectX, `GL_ARB_gl_spirv` to consume the same SPIR-V the thin tier uses | M |

**Done when:** no bind-to-edit call remains where 4.5 offers a named equivalent, and the matrix
still passes.

---

## Stage 3 — DirectX 11 to parity with OpenGL

Currently 8 of ~35 interface methods and none of the 14 resource types.

| # | Task | Effort |
|---|---|---|
| 3.1 | `D3D11Buffer` base plus vertex, index, uniform (constant), storage (UAV) and pixel buffers | M |
| 3.2 | `D3D11ShaderProgram`: compile HLSL with `d3dcompiler`, reflect with `ID3D11ShaderReflection` | M |
| 3.3 | `D3D11VertexAttributeBindings`: `ID3D11InputLayout` built from the vertex shader's reflected signature | M |
| 3.4 | Draw path: `IASetVertexBuffers`, `IASetIndexBuffer`, `IASetPrimitiveTopology`, `Draw`/`DrawIndexed` | S |
| 3.5 | Render states as cached state objects: rasterizer, blend, depth-stencil — DirectX 11 wants objects where OpenGL takes individual calls | M |
| 3.6 | `D3D11Texture2D` and `D3D11TextureSampler`, with shader resource views | M |
| 3.7 | Name-based binding from 1.4, using reflection to resolve names to slots | M |
| 3.8 | `D3D11Framebuffer`: render-target textures and their views | M |
| 3.9 | Compute shaders and UAV binding | M |

**Done when:** every example that runs on OpenGL runs on DirectX 11.

---

## Stage 4 — The thin interface

Design is in ARCHITECTURE.md; this writes it down as headers.

| # | Task | Effort |
|---|---|---|
| 4.1 | `IThinDevice`, `IThinQueue`, adapter enumeration and capability query | M |
| 4.2 | `IThinCommandPool`, `IThinCommandList` — recording, begin/end, reset | M |
| 4.3 | `IThinFence` (timeline), submission with wait/signal values | S |
| 4.4 | `IThinBuffer`, `IThinTexture`, views, and an allocator interface | M |
| 4.5 | `ResourceState` enum and explicit barriers | M |
| 4.6 | `IThinPipeline`, `IThinPipelineLayout`, `IThinShaderModule` | M |
| 4.7 | `IThinDescriptorPool`, `IThinDescriptorSet` | M |
| 4.8 | `IThinSwapchain`: acquire, present, resize | S |
| 4.9 | Dynamic rendering: begin/end rendering with attachment descriptions | S |
| 4.10 | Ray tracing: acceleration structures, RT pipelines, shader binding table | L |

---

## Stage 5 — Vulkan on the thin interface

The backend already contains all of these objects internally; this exposes them.

| # | Task | Effort |
|---|---|---|
| 5.1 | Lift `CommandBuffer`, `CommandPool`, `Fence`, `Semaphore`, `Swapchain`, `LogicalDevice`, `PhysicalDevice`, `QueueFamily` onto the thin interfaces | M |
| 5.2 | Replace render-pass objects with dynamic rendering | M |
| 5.3 | Convert binary semaphores to timeline, except swapchain acquire | S |
| 5.4 | Expose `Pipeline`/`PipelineKey` as `IThinPipeline`; the key becomes the caller's description | M |
| 5.5 | Expose descriptor pool and sets directly | M |
| 5.6 | Move ray tracing over unchanged | M |
| 5.7 | Retire the classic Vulkan backend once the thin examples match it | S |

---

## Stage 6 — DirectX 12 on the thin interface

| # | Task | Effort |
|---|---|---|
| 6.1 | Device, adapter, queues, `d3dx12.h` from the history at `2635b41` | M |
| 6.2 | Command allocator and list, fence-based submission | M |
| 6.3 | `IDXGISwapChain3` with flip-model presentation | S |
| 6.4 | Committed and placed resources, descriptor heaps | L |
| 6.5 | Root signatures from the pipeline layout description | M |
| 6.6 | Pipeline state objects | M |
| 6.7 | Barriers from the shared `ResourceState` enum | M |
| 6.8 | DXR: acceleration structures, RT pipelines, shader binding table | L |

Salvage from history where it helps — the resource, texture and shader files at `2635b41` are
real DirectX 12 code, though written against the older interface and its bind-to-edit model.

---

## Stage 7 — Shader pipeline

| # | Task | Effort |
|---|---|---|
| 7.1 | CMake function `longbow_add_shaders` compiling at build time into the runtime output | M |
| 7.2 | HLSL → DXIL (`dxc`) and HLSL → SPIR-V (`dxc -spirv`) for the thin tier | S |
| 7.3 | HLSL → DXBC (`d3dcompiler` or `fxc`) for DirectX 11 | S |
| 7.4 | GLSL kept as source for OpenGL; optional `glslc` → SPIR-V for `GL_ARB_gl_spirv` | S |
| 7.5 | Optional GLSL → HLSL via `glslc` + `spirv-cross` for single-source examples | M |
| 7.6 | Small runtime loader that picks the right compiled form per backend | S |

---

## Stage 8 — Examples, both tiers, both languages

Two sets, so that each interface is demonstrated in its own idiom rather than one being made to
look like the other.

**Classic** (`source/Examples/Classic/`) — OpenGL and DirectX 11, GLSL and HLSL side by side:

| Example | Shows |
|---|---|
| 01_HelloWindow | Window, clear, present |
| 02_Triangle | Vertex buffer, shader program, draw |
| 03_Cube | Index buffer, uniform buffer, depth test |
| 04_Textures | Texture, sampler, binding by name |
| 05_RenderStates | Blending, culling, stencil |
| 06_Framebuffer | Off-screen target, read-back |
| 07_MeshRendering | Model and material loading |
| 08_ComputeShader | Storage buffers, dispatch |
| 09_Instancing | Instanced draw |
| 10_ImGui | Dear ImGui, from the TOF backend |

**Thin** (`source/Examples/Thin/`) — Vulkan and DirectX 12, HLSL throughout:

| Example | Shows |
|---|---|
| 01_HelloTriangle | Device, swapchain, command list, pipeline |
| 02_UniformBuffers | Descriptor sets, per-frame buffers |
| 03_Textures | Texture upload, barriers, samplers |
| 04_MultiFrame | Frames in flight, timeline synchronisation |
| 05_Compute | Compute pipeline, UAV/storage |
| 06_DynamicRendering | Attachments without render-pass objects |
| 07_Instancing | Instanced and indirect draws |
| 08_ShadowMap | Depth-only pass, two-pass rendering |
| 09_PathTracing | Ray tracing, acceleration structures, SBT |

Optional, once both tiers are stable: the 11 globe-rendering apps from
`spherical-wavelets-for-globe-rendering`, and `05_SceneRenderer` from TOF.

---

## Stage 9 — Bring back what was left behind

| # | Task | Effort |
|---|---|---|
| 9.1 | `NetworkDevice` + a WinSock backend from this repo's history, with the plugin name fixed | M |
| 9.2 | `GameFoundation` (actor/component) from history, this time actually in the build | M |
| 9.3 | Dear ImGui as its own module rather than compiled into a backend, as TOF had it | M |

---

## Stage 10 — Public release

| # | Task | Effort |
|---|---|---|
| 10.1 | Cross-platform plugin loading (`dlopen`) — the loader is still Windows-only | M |
| 10.2 | German doc comments and inline comments to English | M |
| 10.3 | Source files still in Latin-1 to UTF-8 (they raise C4828 today) | S |
| 10.4 | README: what it is, screenshots, build per platform, the two-tier architecture, backend matrix | M |
| 10.5 | `LICENSE` (MIT) — missing entirely | S |
| 10.6 | Doxygen configuration repaired; it points at another machine's paths | S |
| 10.7 | GitHub Actions building Windows, and Linux for OpenGL and Vulkan | M |
| 10.8 | Tests: math, resource loading, and a headless smoke test per backend | M |
| 10.9 | Delete `dependencies/` (22 MB of prebuilt GLEW/GLFW, referenced by nothing) | S |

---

## Order of work

Stages 1 and 2 first: they are cheap and everything downstream is cleaner for them. Stage 3 then
delivers the second classic backend, at which point the classic tier is complete and provable.
Stage 4 can be designed in parallel but is only worth writing once the classic split has settled,
because it borrows vocabulary from what stage 1 removes. Stages 5 and 6 are the large ones;
Vulkan first, since its backend already has the shapes and will expose the design's mistakes
before DirectX 12 has to repeat them.
