# OpenComposite - Rever VR builds (GPL-3.0)

**English** · [Español](#español)

OpenComposite (<https://gitlab.com/znixian/OpenOVR>, branch `openxr`) runs OpenVR games on OpenXR. Rever VR
Central ships three builds of it with Rever's changes. OpenComposite is **GPL-3.0** (`LICENSE`; `LICENSE_INIH.txt`
and `LICENSE_OPENVR.txt` cover the bundled inih and OpenVR headers). This folder is the Corresponding Source of those
three binaries (GPL-3.0 section 6).

## Contents

| Path | What |
| --- | --- |
| `BASE_COMMIT` | Upstream commit every build starts from: `cff07db75c4823afe93ed7027b03d5f7bc86f164`. |
| `source/OpenOVR-cff07db-with-submodules.tar.gz` | That commit's tree with its submodules at the pinned commits: `libs/glm` `bf71a834` (MIT / Happy Bunny), `libs/openxr-sdk` `91a8a8d9` (Apache-2.0). |
| `patches/` | Rever's changes, applied in the order below. |
| `build-helpers/` | `atlbase.h` (lets the build work without Visual Studio's ATL component); `vulkan32/`, `vulkan32_fnv/`: the 32-bit `vulkan-1.lib` import-library stubs (the Vulkan SDK has no 32-bit one). Rever's own files, released under GPL-3.0 with the rest. |
| `REVER_README_*.md` | Rever's detailed notes for each build. |

## The shipped binaries and how each is built

| Shipped file (app) | Arch | Patches, in order | Build helper |
| --- | --- | --- | --- |
| `tools/opencomposite/vrclient_x64.dll` | x64 | `rever_opencomposite.patch`, `rever_opencomposite_gaze.patch` | `atlbase.h` |
| `tools/opencomposite/vrclient.dll` | x86 | `rever_opencomposite.patch`, `rever_opencomposite_x86_alien.patch` | `atlbase.h`, `vulkan32/` |
| `tools/opencomposite_fnv/vrclient.dll` | x86 | `rever_opencomposite.patch`, `rever_opencomposite_x86_alien.patch`, `rever_opencomposite_fnv.patch`, `rever_opencomposite_fnv_laser.patch` | `atlbase.h`, `vulkan32_fnv/` |

`tools/opencomposite/steamvr_standin/vrmonitor.exe` is a separate Rever program (not OpenComposite, not linked to it)
and is not part of this source.

Steps (Visual Studio 2026, CMake, Vulkan SDK; details in `REVER_README_x64_x86.md`):

```bat
tar xzf source\OpenOVR-cff07db-with-submodules.tar.gz
cd OpenOVR
git init -q && git apply ..\patches\rever_opencomposite.patch
rem then the build's other patches, in the table's order, e.g. for the x64 build:
git apply ..\patches\rever_opencomposite_gaze.patch
rem Vulkan SDK: copy Include and Lib\vulkan-1.lib into libs\vulkan\Include and libs\vulkan\Lib
set CL=/I"%CD%\..\build-helpers"
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release --target OCOVR
rem -> build\bin\Release\vrclient_x64.dll
```

x86: build `vulkan-1.lib` from `build-helpers/vulkan32/` (or `vulkan32_fnv/` for New Vegas) as
`REVER_README_x64_x86.md` explains, copy it to `libs/vulkan/Lib32/`, then configure with `-A Win32` -> `vrclient.dll`.

New Vegas: the upstream tree has CRLF line endings and the two New Vegas patches were written with LF, so `git apply`
refuses them. Apply those two with GNU patch: `patch -p1 --fuzz=3 < ..\patches\rever_opencomposite_fnv.patch`, then
the same for `rever_opencomposite_fnv_laser.patch`. Checked on 2026-11-07: the result matches the tree the shipped
`opencomposite_fnv/vrclient.dll` was built from, line endings and one blank line aside.

## How it relates to Rever VR Central

The app (closed source) never links OpenComposite. It copies `vrclient*.dll` into an OpenVR runtime folder and lists
that folder in `%LOCALAPPDATA%\openvr\openvrpaths.vrpath`; the game loads it. Rever's OpenXR layers and runtime are
reached by OpenComposite only through the standard OpenXR loader, plus two optional, documented hand-overs that
appear in these patches: a `rever_session_env.json` file read at start (environment variables), and, in the x64 gaze
build, the optional `rever_streamer_gaze_dirs` export of Rever's runtime when that runtime is present.

---

## Español

OpenComposite (GPL-3.0) traduce OpenVR a OpenXR. Rever VR Central lleva tres builds con cambios de Rever; esta carpeta
es su código fuente correspondiente (sección 6 de la GPL-3.0):

- `source/`: el commit de upstream `cff07db` con sus submódulos (glm, OpenXR-SDK) en los commits fijados.
- `patches/`: los cambios de Rever, en el orden de la tabla de arriba según el binario.
- `build-helpers/`: `atlbase.h` y los stubs de `vulkan-1.lib` de 32 bits (archivos propios de Rever, también GPL-3.0).
- `REVER_README_*.md`: las notas de cada build.

| Binario de la app | Parches, en orden |
| --- | --- |
| `tools/opencomposite/vrclient_x64.dll` (x64) | `rever_opencomposite.patch`, `rever_opencomposite_gaze.patch` |
| `tools/opencomposite/vrclient.dll` (x86) | `rever_opencomposite.patch`, `rever_opencomposite_x86_alien.patch` |
| `tools/opencomposite_fnv/vrclient.dll` (x86) | los dos anteriores + `rever_opencomposite_fnv.patch` + `rever_opencomposite_fnv_laser.patch` |

La app (código cerrado) no enlaza OpenComposite: copia la DLL a una carpeta de runtime OpenVR y el juego la carga. Las
capas y el runtime de Rever solo se tocan por el cargador OpenXR estándar, por el archivo `rever_session_env.json` y,
en el build x64 con mirada, por la función opcional `rever_streamer_gaze_dirs` del runtime de Rever.
