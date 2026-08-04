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

| Backend | Tier | Beispiele | Anmerkung |
|---|---|---|---|
| OpenGL 4.5 | Classic | **9/9** | Core Profile, Direct State Access durchgehend, Debug-Callback |
| Vulkan 1.3 | Thin (künftig) | **9/9** | inkl. Raytracing; läuft noch über die klassische Fassade |
| DirectX 11 | Classic | **7/9** | offen: 01_Input (GLFW-Input am rohen HWND), 09 (kein Raytracing) |
| DirectX 12 | Thin (künftig) | 0/9 | unverändert Rumpf aus vier Dateien |

### Stufen

| Stufe | Stand |
|---|---|
| 1 — Schnittstelle aufteilen | offen (bewusst zurückgestellt, siehe unten) |
| 2 — OpenGL 4.5 | **fertig** |
| 3 — DirectX 11 | **fertig bis auf Input und Raytracing** |
| 4 — Thin-Schnittstelle | offen |
| 5 — Vulkan auf Thin | offen |
| 6 — DirectX 12 auf Thin | offen |
| 7 — Shader-Pipeline zur Buildzeit | offen (Übersetzung läuft zur Laufzeit im DX11-Backend) |
| 8 — Beispiele Classic/Thin, GLSL/HLSL | offen |
| 9 — NetworkDevice, GameFoundation, ImGui | offen |
| 10 — Öffentlichkeit | teilweise: **LICENSE und README fertig**, Rest offen |

## Jetzt dran

Reihenfolge bewusst geändert: Stufe 1 würde Vulkan und `09_PathTracing` über mehrere Stufen
zerlegen, weil beide Raytracing über die klassische Schnittstelle nutzen. Erst wenn Stufe 4
und 5 fertig sind, ist der Schnitt schmerzfrei.

### Rest von Stufe 3
- [ ] Win32-Input-Backend, damit `01_Input` auf DirectX 11 läuft. Der Input geht über GLFW,
      das DirectX-Fenster ist ein rohes HWND — entweder ein `RawInput`-Backend oder das
      DirectX-Fenster auf GLFW umstellen (GLFW kann `glfwGetWin32Window`)
- [ ] `09_PathTracing` soll auf DX11 „kein Raytracing" melden statt
      „shader-creation-failed" — die Meldung entsteht, weil das Beispiel abbricht, bevor
      der RT-Pfad überhaupt erreicht wird

### Dann Stufe 4 und 5
Die Thin-Schnittstelle entwerfen und Vulkan darauf heben. Der Vulkan-Backend enthält die
Objekte bereits intern (`CommandBuffer`, `CommandPool`, `Fence`, `Semaphore`, `Pipeline`,
`RenderPass`, `Swapchain`, `LogicalDevice`, `PhysicalDevice`, `QueueFamily`, `DeviceMemory`),
sie müssen nur nach außen gelegt werden. Details in [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

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
