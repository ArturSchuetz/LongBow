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

## Nächste Session

- [ ] Phase 1 abschließen (siehe unten)

## Offen

### Phase 1 — Skelett
- [ ] Zielstruktur `source/<Modul>/{include/<Modul>,source}` anlegen
- [ ] Root-`CMakeLists.txt` neu schreiben (CMake ≥ 3.21, C++17, Optionen pro Backend)
- [ ] `CMakePresets.json` (msvc-x64-debug/release)
- [ ] `.clang-format`, `.gitignore` überarbeiten (aktuell werden `bin/`, `lib/`, `doc/` ignoriert
      und einzelne Dateien force-added)
- [ ] `CoreSystems` + `Platform` migrieren und **bauend** bekommen — erster Meilenstein

### Phase 2 — Kernmodule
- [ ] `Resources` (Bild-/Mesh-/Material-Loader, LoadPNG)
- [ ] `RenderDevice`, `InputDevice`, `NetworkDevice` (Interfaces + Manager)
- [ ] Plugin-Lader plattformunabhängig machen (aktuell `LoadLibraryExW` hart auf Windows)

### Phase 3 — Backends
- [ ] OpenGL 3.x lauffähig (Referenz-Backend)
- [ ] DirectX 12 lauffähig — offene Enden: `VCreateVertexArray(mesh,…)` und `VCreateFramebuffer`
      geben `nullptr` zurück
- [ ] DirectX 11 aus TOF übernehmen
- [ ] Vulkan aus `long-bow-engine` übernehmen (inkl. Raytracing)
- [ ] Backend-Auswahl zur Laufzeit über eine gemeinsame `IBowRenderDevice`-API verifizieren

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
