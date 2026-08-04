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
`09_PathTracing` über mehrere Stufen zerlegen, weil beide Raytracing über die klassische
Schnittstelle nutzen. Stufe 3 ist reine Ergänzung und bricht nichts, kommt also zuerst.

### Fertig

- [x] Buffer: `D3D11Buffer` als Basis, Vertex-, Index- und Constant-Buffer darauf.
      Dynamic wird gemappt (DISCARD), Default geht über `UpdateSubresource`,
      Read-back über eine Staging-Kopie
- [x] **Shader-Übersetzung GLSL → SPIR-V → HLSL → DXBC** über shaderc und SPIRV-Cross aus dem
      Vulkan SDK. Damit laufen die vorhandenen GLSL-Beispiele unverändert auf DirectX 11.
      HLSL wird durchgereicht, erkannt am fehlenden `#version`.
      SPIRV-Cross korrigiert dabei Clip-Space und Y-Richtung, weil GLSL die Tiefe von -1..1
      und den Ursprung unten hat, DirectX 0..1 und oben
- [x] `D3D11ShaderProgram`: kompiliert beide Stufen, reflektiert Vertex-Eingänge und
      Ressourcen-Slots über `ID3D11ShaderReflection`, hält den Vertex-Bytecode fürs Input Layout
- [x] Push Constants werden auf Constant Buffer abgebildet, adressiert über den Namen
- [x] Texturen (mit Verbreiterung von 3 auf 4 Kanäle), Sampler als State-Objekte,
      `D3D11ShaderResourceBindings`
- [x] Alles im Gerät verdrahtet

### Genau hier weitermachen

- [ ] **`D3D11VertexAttributeBindings`** — das fehlende Stück.
      `VCreateVertexAttributeBindings` gibt noch `nullptr` zurück, deshalb stürzt
      `03_Triangle` ab: nicht das Backend, sondern das Beispiel dereferenziert den Nullzeiger.
      Gebraucht wird:
    - ein `ID3D11InputLayout`, erzeugt aus `D3D11ShaderProgram::GetVertexByteCode()`
      und den gesetzten Attributen
    - die Semantik-Zuordnung: SPIRV-Cross benennt übersetzte Eingänge `TEXCOORD<n>` und
      legt die GLSL-Location in den Semantic Index. `ReflectVertexInput` wertet das bereits
      aus und legt die Attribute unter dieser Location ab
- [ ] Draw-Pfad im Kontext: `IASetVertexBuffers`, `IASetIndexBuffer`,
      `IASetPrimitiveTopology`, `Draw`/`DrawIndexed`, davor `Bind()` des Shader-Programms
- [ ] Render-States als gecachte Objekte (Rasterizer, Blend, DepthStencil) —
      DirectX will Objekte, wo OpenGL Einzelaufrufe nimmt
- [ ] Ressourcen im Kontext binden: Namen über die Reflection in Slots auflösen, dann
      `VSSetConstantBuffers`, `PSSetShaderResources`, `PSSetSamplers`
- [ ] Framebuffer über Render-Target-Views
- [ ] Compute und Storage Buffer (UAV)

Stand auf DirectX 11: `02_HelloWorld` läuft fehlerfrei (1107 Present-Aufrufe im Testlauf).
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
