# TODO

Umbau von LongBow zu einem öffentlich präsentierbaren, CMake-basierten Projekt.
Branch: `modernize/cmake-merge`. Ziel-Repo bleibt `github.com/ArturSchuetz/LongBow`.

## Entscheidungen (getroffen)

- **Fundament:** Modul-Layout und Quellcode aus `long-bow-engine` (C++17), CMake aber **neu
  geschrieben** — das cmake-init-Template dort konfiguriert unter CMake 4.x nicht mehr
  (`optick` fordert `cmake_minimum_required < 3.5`, Export-Sets für `glfw`/`OptickCore` kaputt).
- **Backends:** alle vier — OpenGL 3.x, DirectX 11, DirectX 12, Vulkan.
- **Git:** Feature-Branch, am Ende PR gegen `master`. Nicht pushen ohne Rückfrage.
- **Sprache:** Code-Kommentare und Doku auf Englisch (öffentliches Repo).
- **ThirdParty:** nicht die ~350 MB aus `long-bow-engine` übernehmen. glslang/shaderc/SPIRV-*
  kommen aus dem Vulkan SDK, glfw/glew/optick über FetchContent, nur LoadPNG bleibt vendored.

## Quellen pro Baustein

| Baustein | Bestes Vorbild | Umfang |
|---|---|---|
| CMake-/Modul-Struktur | `long-bow-engine` | `source/<Modul>/include/<Modul>/` |
| OpenGL 3.x | `Time-Of-Flight-Sensor-Simulation` (67 Dateien, inkl. ImGui) + Compute/UBO/SSBO aus `long-bow-engine` | groß |
| DirectX 12 | **dieses Repo** (41 Dateien) | groß, unfertig |
| DirectX 11 | `Time-Of-Flight-Sensor-Simulation` (21 Dateien) | mittel |
| Vulkan | `long-bow-engine` (79 Dateien, inkl. Raytracing) | groß |
| Beispiele | `long-bow-engine` (00–09) | mittel |
| Globe-Rendering-Apps | `spherical-wavelets-for-globe-rendering` (11 Apps) | optional |

## Testmatrix (Stand: alle Beispiele × alle Backends, Debug-Build)

Ausgeführt mit `LONGBOW_BACKEND=<api>`, 12 s Timeout, Klassifikation über den Log.

| Beispiel | OpenGL 3.x | DirectX 12 | Vulkan |
|---|---|---|---|
| 01_Input | läuft (kein Rendering, so gewollt) | Stub | rendert |
| 02_HelloWorld | **rendert** | Stub | rendert |
| 03_Triangle | Resource-Bindings fehlen | Stub | rendert |
| 04_Cube | Resource-Bindings fehlen | Stub | rendert |
| 05_Textures | Resource-Bindings fehlen | Stub | rendert |
| 06_MeshRendering | Resource-Bindings fehlen | Stub | rendert |
| 07_Framebuffer | Resource-Bindings fehlen | Stub | rendert |
| 08_ComputeShader | Compute fehlt | Stub | rendert |
| 09_PathTracing | kein Raytracing in OpenGL (korrekt) | Stub | rendert |

**Vulkan 9/9. OpenGL 2/9. DirectX 12 0/9.**

DirectX 12 scheitert immer an derselben Stelle: `VCreateWindow` in
`BowDirectX12RenderDevice.cpp:93` ist `LOG_FATAL("Not yet implemented!")`. Das aus
`long-bow-engine` übernommene DX12-Backend besteht aus **vier Dateien** — es ist ein Rumpf.
Die substanzielle 41-Dateien-Implementierung lag im alten Baum dieses Repos und ist über die
Git-Historie erreichbar (`git show 2635b41:src/Engine/RenderDeviceImplementations/DirectX12RenderDevice/`).
Sie implementiert allerdings die alte API (`IBowVertexArray`) und muss portiert werden.

## Nächste Session

- [ ] `dependencies/` löschen (22 MB vorgebautes GLEW/GLFW für Windows, wird durch FetchContent
      ersetzt und von nichts mehr referenziert). Mein Löschversuch wurde vom
      Berechtigungs-Klassifikator blockiert — muss von Hand passieren:
      `git rm -r dependencies`
- [ ] **OpenGL lauffähig machen** — es fehlen genau fünf Funktionen in
      `OpenGL3xRenderDevice/source/Device/Shader/BowOGL3xShaderProgram.cpp`, alle mit
      `LOG_FATAL("Not yet Implemented")`. OpenGL initialisiert sonst sauber durch (GLFW, GLEW,
      Shader-Compile, Attribut-Reflection) und bricht erst bei `VCreateResourceBindingObjects` ab:
    - `VCreateResourceBindingObjects()` (Zeile 116)
    - `FindShaderResources(uint32_t program)` (237) — Uniforms/Sampler/UBOs reflektieren
    - `VSetTexture(name, texture, sampler)` (345)
    - `VSetPushConstants(name, data, offset, size)` (352) — auf Uniforms abbilden
    - `VSetPushConstants(shaderStage, data, offset, size)` (359)
- [ ] **DirectX 12 portieren** — das übernommene Backend ist ein Rumpf aus vier Dateien.
      Die echte Implementierung aus der Historie holen und auf die neue API heben
      (`IBowVertexArray` → `IBowVertexAttributeBindings`, Resource Bindings, Push Constants)
- [ ] DirectX 11 aus TOF portieren

## Offen

### Phase 2 — Kernmodule
- [ ] `NetworkDevice` + `WinSockNetworkDevice` aus dem alten Repo übernehmen (fehlen in
      `long-bow-engine` komplett)
- [ ] `GameFoundation` (Actor/Component) aus dem alten Repo übernehmen
- [ ] Plugin-Lader plattformunabhängig machen (immer noch `LoadLibrary`, trotz
      "cross-platform" in der README von `long-bow-engine`)
- [ ] Logger-Injektion über DLL-Grenzen prüfen — beim Start erscheint
      "WARNING: Logger instance already exists, but it is different from the one passed as argument"

### Phase 3 — Backends
- [ ] DirectX 11 aus TOF übernehmen
- [ ] DirectX 12 tatsächlich lauffähig — baut, aber `VCreateBottomLevelAccelerationStructure`
      und `VCreateTopLevelAccelerationStructure` melden "Not yet implemented"
- [ ] Bestes OpenGL aus TOF (67 Dateien, inkl. Dear ImGui) gegen den jetzigen Stand abgleichen
- [ ] Backend zur Laufzeit wählbar machen (Kommandozeile/Env) statt fest einkompiliert —
      aktuell steht in jedem Beispiel `RenderDeviceAPI::Vulkan` im Quelltext
- [ ] Absolute Pfade aus den Beispielen entfernen, z. B.
      `F:/Projects/masterthesis/data/Scenes/Sponza/...` in `05_Textures/main.cpp`
- [ ] `05_Textures` erzeugt Textur und Sampler, bindet sie aber nie — Beispiel unvollständig

### Phase 4 — Bugs aus dem Altbestand
- [ ] `NetworkDeviceManager` lädt `NetworkDevice[_d].dll`, gebaut wird `WinSockNetworkDevice[_d].dll`
- [ ] `include/BowScene.h` inkludiert `IBowRenderStrategy.h` + `BowLegacyVertexLitRenderer.h` —
      beide existieren nicht
- [ ] `GameFoundation` hat Quellen und ein `.vcxproj`, ist aber in keiner Solution
- [ ] `Scene`-Projekt ist in der Solution, enthält aber keine Quelldatei
- [ ] `math::Sqrt` in `BowMath.h` ist der Quake-Trick für die **inverse** Wurzel, wird aber als
      `Sqrt` zurückgegeben; `double`-Overload puned zusätzlich durch `long`
- [ ] `Doxyfile`: absolute Pfade einer fremden Maschine (`C:/Users/Greg/...`)
- [ ] `Viewport::operator==` vergleicht `x` gegen alle Felder statt Feld gegen Feld
      (`IBowRenderContext.h`)

### Phase 5 — Übersetzung
- [ ] Deutsche Doxygen-Blöcke (`\~german`) nach Englisch
- [ ] Deutsche Inline-Kommentare nach Englisch

### Phase 6 — Öffentlichkeit
- [ ] Ausführliche `README.md`: Was es ist, Screenshots, Build pro Plattform, Architektur,
      Backend-Matrix, Beispiele, Lizenz
- [ ] `LICENSE` (MIT, Artur Schütz) — fehlt bisher komplett
- [ ] `CONTRIBUTING.md`
- [ ] Doxygen-Konfiguration reparieren, Ausgabe nach `docs/`
- [ ] GitHub Actions: Build Windows (+ Linux, falls Backends es hergeben)
- [ ] `CLAUDE.md` auf den neuen Zustand aktualisieren

### Phase 7 — Beispiele
- [ ] Beispiele 00–09 aus `long-bow-engine` übernehmen
- [ ] Prüfen, welche Samples aus diesem Repo noch Eigenwert haben (Quaternions, MeshRendering)
- [ ] Globe-Rendering-Apps aus `spherical-wavelets-…` — nur falls sie ohne großen Aufwand mitlaufen

## Ideen / später

- Optick-Profiling optional wieder aktivieren
- Dear ImGui aus dem OpenGL-Backend herauslösen (in TOF ist es dort einkompiliert)
- `bin/Data` (Sponza, Corvette, SU-27) auf Lizenz prüfen, bevor es öffentlich bleibt
- Vulkan-Raytracing-Beispiel (09_PathTracing) als Schaufenster für die README

## Erledigt

- [x] Bestandsaufnahme aller sieben LongBow-Varianten unter `F:\Projects`
- [x] `CLAUDE.md` für den Ist-Zustand geschrieben
- [x] Toolchain geprüft: CMake 4.2, MSVC 14.44, Vulkan SDK 1.4.341.1, Windows SDK 10.0.26100 —
      **v140 fehlt, die alte `.sln` baut auf dieser Maschine nicht**
- [x] Branch `modernize/cmake-merge` angelegt
- [x] Phase 1: Skelett aus `long-bow-engine` übernommen, CMake komplett neu geschrieben
      (`cmake/LongBowModule.cmake` statt ~200 Zeilen Zeremonie pro Modul)
- [x] `.gitignore` neu — die alte ignorierte `*.txt` und hätte **jede CMakeLists.txt**
      stillschweigend aus dem Repo gehalten
- [x] ThirdParty von ~350 MB auf LoadPNG (452 KB) reduziert; glslang/shaderc/SPIRV aus dem
      Vulkan SDK, glfw/glew/optick über FetchContent
- [x] Optick-Stub, damit die ~300 `OPTICK_EVENT`-Stellen auch ohne Profiler bauen
- [x] Drei latente CoreSystems-Bugs behoben (Sphere-Konstruktor, Matrix3x3-Komma,
      `math::Sqrt` → `ReciprocalSqrt`)
- [x] Vier Vulkan-Bugs behoben (Rückgabetyp-Mismatches, redundanter Klassen-Qualifier)
- [x] OpenGL-Backend wieder an die aktuelle `IRenderDevice`-Schnittstelle angeglichen
- [x] Alle zehn Beispiele bauen; **Vulkan rendert nachweislich** (03_Triangle läuft im
      Frame-Loop), OpenGL initialisiert bis zum Shader-Resource-Binding
- [x] `07_Framebuffer` auf Resource-Bindings portiert (war mit `assert(!"Uniforms are gone!")`
      liegengelassen)
- [x] Altbestand entfernt: `src/`, `include/`, `vs12/`, `bin/` (566 Dateien, 643k Zeilen).
      `doc/` bleibt — die README verlinkt die zwei Screenshots. Kleine Test-Texturen nach
      `data/` gerettet. Sponza (128 MB), NASA-Texturen (~155 MB), Corvette und SU-27 sind
      raus, aber über die Historie erreichbar
- [x] Branch nach GitHub gepusht: `modernize/cmake-merge`
