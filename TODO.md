# TODO

Umbau von LongBow zu einem öffentlich präsentierbaren, CMake-basierten Projekt mit **zwei**
Renderer-Schnittstellen. Branch: `modernize/cmake-merge`.

Der ausführliche Plan steht in [docs/PLAN.md](docs/PLAN.md), die Begründung der Architektur in
[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), die Bestandsaufnahme in
[docs/INVENTORY.md](docs/INVENTORY.md). Diese Datei ist die Arbeitsliste.

## Entscheidungen

- **Zwei Tiers.** Classic (OpenGL 4.5, DirectX 11) und Thin (Vulkan 1.3, DirectX 12) mit je
  eigener Schnittstelle, statt beides in eine zu pressen.
- **Thin API:** Dynamic Rendering statt Render-Pass-Objekten, ausschließlich Timeline-Semaphoren,
  expliziter `ResourceState` mit Barrieren, Descriptor Sets als Tabellen.
- **Shader:** HLSL für Thin (dxc erzeugt DXIL *und* SPIR-V), GLSL für OpenGL, HLSL für DX11.
  Übersetzung zur Buildzeit. Beispiele existieren bewusst in beiden Sprachen.
- **ThirdParty:** nichts Neues. dxc, glslc, spirv-cross kommen aus dem Vulkan SDK, d3dcompiler
  aus dem Windows SDK.
- Git: Feature-Branch, PR gegen `master`. Sprache im Code: Englisch.

## Stand

| Backend | Tier | Stand |
|---|---|---|
| OpenGL | Classic | 9/9 Beispiele, 4.5-Core-Kontext, Buffer auf DSA. Texturen/Framebuffer/VAOs noch 3.x |
| Vulkan | Thin (künftig) | 9/9 Beispiele, inkl. Raytracing. Läuft noch über die klassische Fassade |
| DirectX 11 | Classic | Gerät, Fenster, Swap Chain, Clear, Present. **Keine Ressourcentypen** |
| DirectX 12 | Thin (künftig) | Rumpf aus 4 Dateien. Echte Implementierung in der Historie (`2635b41`) |

## Jetzt dran

### Stufe 1 — Schnittstelle aufteilen
- [ ] Modul `ThinRenderDevice` anlegen
- [ ] Raytracing aus `RenderDevice` herausziehen
- [ ] `VBeginFrame`/`VEndFrame` und `VSetPushConstants` aus der klassischen Schnittstelle
- [ ] `IShaderResourceBindings` im Classic-Tier durch Binden über Namen am Kontext ersetzen
- [ ] OpenGL-Backend nachziehen
- [ ] `ThinRenderDeviceManager`

### Stufe 2 — OpenGL 4.5 durchgängig
- [ ] Texturen auf `glCreateTextures`/`glTextureStorage2D`/`glTextureParameteri`
- [ ] Framebuffer auf `glCreateFramebuffers`/`glNamedFramebufferTexture`
- [ ] Vertex Arrays auf `glCreateVertexArrays`/`glVertexArrayAttribFormat`
- [ ] `glBindTextureUnit` statt `glActiveTexture` + `glBindTexture`
- [ ] Modul und Klassen von `OpenGL3x`/`OGL3x` auf `OpenGL`/`OGL` umbenennen

### Stufe 3 — DirectX 11 auf Augenhöhe mit OpenGL
- [ ] Buffer (Vertex, Index, Constant, Storage/UAV, Pixel)
- [ ] Shader-Programm über `d3dcompiler` + `ID3D11ShaderReflection`
- [ ] Input Layout aus der Vertex-Shader-Signatur
- [ ] Draw-Pfad
- [ ] Render-States als gecachte State-Objekte
- [ ] Texturen und Sampler
- [ ] Binden über Namen
- [ ] Framebuffer
- [ ] Compute

## Danach

- Stufe 4: Thin-Schnittstelle entwerfen und schreiben
- Stufe 5: Vulkan darauf heben (die Objekte existieren dort bereits intern)
- Stufe 6: DirectX 12 darauf schreiben, mit der Historie als Steinbruch
- Stufe 7: Shader-Pipeline zur Buildzeit
- Stufe 8: Beispiele, 10 für Classic und 9 für Thin
- Stufe 9: `NetworkDevice`, `GameFoundation`, ImGui als eigenes Modul zurückholen
- Stufe 10: Linux-Plugin-Laden, Übersetzung, README, LICENSE, CI, Tests

## Offene Kleinigkeiten

- [ ] `dependencies/` löschen (22 MB, referenziert von nichts) — mein Versuch wurde vom
      Berechtigungs-Klassifikator blockiert: `git rm -r dependencies`
- [ ] Logger-Injektion über DLL-Grenzen: beim Start erscheint "Logger instance already exists,
      but it is different from the one passed as argument"
- [ ] `Viewport::operator==` vergleicht `x` gegen alle Felder statt Feld gegen Feld
- [ ] `01_Input` nutzt GLFW-Input, das DX11-Fenster ist ein rohes HWND — Input-Backend für Win32
- [ ] Einige Quelldateien sind Latin-1 statt UTF-8 (C4828)

## Erledigt

- [x] Bestandsaufnahme aller sieben Repos, verifiziert statt Dateien gezählt
- [x] CMake-Migration; VS2015-Solution entfernt (v140 ist hier gar nicht installiert)
- [x] ThirdParty von ~350 MB auf 452 KB
- [x] `.gitignore` neu — die alte ignorierte `*.txt` und damit jede CMakeLists.txt
- [x] Backend zur Laufzeit wählbar (`--backend` / `LONGBOW_BACKEND`)
- [x] Testmatrix: alle Beispiele gegen alle Backends, Klassifikation über den Log
- [x] OpenGL: Resource-Bindings implementiert, 2/9 → 9/9
- [x] OpenGL: 4.5-Core-Kontext, Debug-Callback, DSA für Buffer, Immediate Mode raus
- [x] OpenGL: Vulkan-GLSL (`layout(set=…)`, `push_constant`) wird übersetzt
- [x] DirectX 11 neu geschrieben — erste lauffähige Scheibe
- [x] Plugin-Loader tabellengetrieben, `GetProcAddress` wird geprüft
- [x] Bugs: Sphere-Konstruktor, Matrix3x3-Komma, `ReciprocalSqrt`, vier Vulkan-Rückgabetypen,
      `malloc`/`delete[]`-Paarung, `glClampColor` mit entfernten Enums
