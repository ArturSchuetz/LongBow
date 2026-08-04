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
| DirectX 11 | **nirgends vorhanden** — neu geschrieben, siehe unten | groß |
| Vulkan | `long-bow-engine` (79 Dateien, inkl. Raytracing) | groß |
| Beispiele | `long-bow-engine` (00–09) | mittel |
| Globe-Rendering-Apps | `spherical-wavelets-for-globe-rendering` (11 Apps) | optional |

## Testmatrix (Stand: alle Beispiele × alle Backends, Debug-Build)

| Beispiel | OpenGL 4.5 | DirectX 11 | DirectX 12 | Vulkan |
|---|---|---|---|---|
| 01_Input | läuft (kein Rendering, so gewollt) | Fenster ok, Input über GLFW | Rumpf | rendert |
| 02_HelloWorld | **rendert** | **präsentiert** | Rumpf | rendert |
| 03_Triangle | **rendert** | braucht Buffer/Shader | Rumpf | rendert |
| 04_Cube | **rendert** | braucht Buffer/Shader | Rumpf | rendert |
| 05_Textures | **rendert** | braucht Texturen | Rumpf | rendert |
| 06_MeshRendering | **rendert** | braucht Buffer/Shader | Rumpf | rendert |
| 07_Framebuffer | **rendert** | braucht Framebuffer | Rumpf | rendert |
| 08_ComputeShader | **rechnet korrekt** | kein Compute in DX11-Rumpf | Rumpf | rechnet |
| 09_PathTracing | kein RT in OpenGL (korrekt) | kein RT in DX11 (korrekt) | Rumpf | rendert |

**OpenGL 9/9 · Vulkan 9/9.** Alle Shader linken, keine Treiber-Diagnostik.

DirectX 11 ist neu geschrieben (es gab nirgends eine lauffähige Implementierung, siehe unten).
Gerät, Fenster, Swap Chain, Clear und Present laufen — 02_HelloWorld präsentiert fehlerfrei.

DirectX 12 bleibt vorerst liegen: das übernommene Backend ist ein Rumpf aus vier Dateien.

### Warum DirectX 11 neu geschrieben werden musste

Die vermeintliche DX11-Implementierung in `Time-Of-Flight-Sensor-Simulation` und
`Masterthesis_Project` (je 1836 Zeilen) ist keine:

- sie referenziert `BowD3D11VertexBuffer.h`, `BowD3D11ShaderProgram.h`, `BowD3D11Texture2D.h`,
  `BowD3D11VertexArray.h`, Framebuffer, Uniform, IndexBuffer, WritePixelBuffer — **keine dieser
  Dateien existiert in irgendeinem Repo**
- ihr Render-Kontext ruft `glDrawRangeElements`, `glDrawArrays` und
  `ImGui_ImplOpenGL3_RenderDrawData` auf: es ist der OpenGL-Kontext mit umbenannter Klasse
- sie inkludiert `d3dx11.h`, von Microsoft entfernt und im aktuellen Windows SDK nicht vorhanden
- TOFs eigene CMakeLists nennt sie „currently unreachable" und baut sie nicht

## Nächste Session

- [ ] `dependencies/` löschen (22 MB vorgebautes GLEW/GLFW, von nichts mehr referenziert).
      Mein Löschversuch wurde vom Berechtigungs-Klassifikator blockiert: `git rm -r dependencies`
- [ ] **DirectX 11 fertig bauen** — Gerät, Fenster, Swap Chain, Clear und Present laufen.
      Es fehlen, in dieser Reihenfolge:
    - Vertex- und Index-Buffer (`ID3D11Buffer`)
    - Shader-Programm: HLSL über `D3DCompile`, Input Layout aus der Vertex-Shader-Reflection
    - Vertex Attribute Bindings (`IASetVertexBuffers` + `ID3D11InputLayout`)
    - Draw-Pfad und Render-States (Rasterizer/Blend/DepthStencil-State-Objekte)
    - Texturen und Sampler, dann Shader Resource Bindings
    - Framebuffer (Render-Target-Texturen)
- [ ] Beispiele-Shader: die Beispiele liefern GLSL. Für DX11 braucht es entweder HLSL-Varianten
      oder eine Übersetzung (SPIRV-Cross kann GLSL → HLSL und liegt im Vulkan SDK)

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
- [ ] DirectX 12 tatsächlich lauffähig — baut, aber `VCreateBottomLevelAccelerationStructure`
      und `VCreateTopLevelAccelerationStructure` melden "Not yet implemented"
- [ ] Bestes OpenGL aus TOF (67 Dateien, inkl. Dear ImGui) gegen den jetzigen Stand abgleichen

### Phase 4 — Bugs aus dem Altbestand
- [ ] `NetworkDeviceManager` lädt `NetworkDevice[_d].dll`, gebaut wird `WinSockNetworkDevice[_d].dll`
- [ ] `include/BowScene.h` inkludiert `IBowRenderStrategy.h` + `BowLegacyVertexLitRenderer.h` —
      beide existieren nicht
- [ ] `GameFoundation` hat Quellen und ein `.vcxproj`, ist aber in keiner Solution
- [ ] `Scene`-Projekt ist in der Solution, enthält aber keine Quelldatei
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
