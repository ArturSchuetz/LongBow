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

## Jetzt dran — Stufe 3, DirectX 11

Reihenfolge bewusst geändert: Stufe 1 (Schnittstelle aufteilen) würde Vulkan und
09_PathTracing über mehrere Stufen zerlegen, weil beide Raytracing über die klassische
Schnittstelle nutzen. Stufe 3 ist reine Ergänzung und bricht nichts, kommt also zuerst.

### Fertig
- [x] Buffer:  als Basis, Vertex-, Index- und Constant-Buffer darauf.
      Dynamic → /, Default → , Read-back über Staging-Kopie
- [x] Shader-Übersetzung: **GLSL → SPIR-V → HLSL → DXBC**, über shaderc und SPIRV-Cross aus dem
      Vulkan SDK. Damit laufen die vorhandenen GLSL-Beispiele unverändert auf DirectX 11.
      HLSL wird durchgereicht (Erkennung über )
- [x] : kompiliert beide Stufen, reflektiert Vertex-Eingänge und
      Ressourcen-Slots über , hält den Vertex-Bytecode fürs Input Layout
- [x] Push Constants → Constant Buffer, per Name
- [x] Texturen, Sampler, 
- [x] Alles im Gerät verdrahtet

### Als Nächstes — genau hier weitermachen
- [ ] **** — das fehlende Stück. 
      gibt noch  zurück, deshalb stürzt  ab (kein Fehler im Backend,
      das Beispiel dereferenziert den Nullzeiger). Braucht:
    -  aus  und den gesetzten Attributen
    - Semantik-Zuordnung: SPIRV-Cross benennt übersetzte Eingänge ,
      die GLSL-Location steckt im Semantic Index —  wertet das schon aus
- [ ] Draw-Pfad: , , ,
      /, dazu  des Shader-Programms
- [ ] Render-States als gecachte Objekte (Rasterizer, Blend, DepthStencil)
- [ ] Ressourcen-Bindung im Kontext: Namen über die Reflection in Slots auflösen,
      //
- [ ] Framebuffer über Render-Target-Views
- [ ] Compute und Storage Buffer (UAV)

Stand der Beispiele auf DirectX 11:  läuft (1107 Present-Aufrufe, fehlerfrei).
Der Rest wartet auf den Draw-Pfad.

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
