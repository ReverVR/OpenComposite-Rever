# OpenComposite - Rever VR builds (GPL-3.0)

**English** · [Español](#español)

OpenComposite (<https://gitlab.com/znixian/OpenOVR>, branch `openxr`) runs OpenVR games on OpenXR. Rever VR
Central ships three builds of it with Rever's changes. OpenComposite is **GPL-3.0** (`LICENSE`; `LICENSE_INIH.txt`
and `LICENSE_OPENVR.txt` cover the bundled inih and OpenVR headers). This folder is the Corresponding Source of the
three binaries Rever VR Central **0.72** ships (GPL-3.0 section 6). Release `0.71` covered 0.71.x; this release adds
the 0.72 changes: Half-Life: Alyx's depth for Rever's layer and the finger gestures (x64), and the New Vegas keyboard
fix (x86, shipped since 0.71.2).

## Contents

| Path | What |
| --- | --- |
| `BASE_COMMIT` | Upstream commit every build starts from: `cff07db75c4823afe93ed7027b03d5f7bc86f164`. |
| `source/OpenOVR-cff07db-with-submodules.tar.gz` | That commit's tree with its submodules at the pinned commits: `libs/glm` `bf71a834` (MIT / Happy Bunny), `libs/openxr-sdk` `91a8a8d9` (Apache-2.0). The same file as release `0.71`. |
| `source/OpenOVR-cff07db-rever-alyx-gestures-20261120-x64.tar.gz` | The exact tree the 0.72 x64 `vrclient_x64.dll` was built from: the base with its full submodules and the four x64 patches applied (build outputs and the Vulkan SDK's files left out). |
| `patches/` | Rever's changes, applied in the order below. |
| `build-helpers/` | `atlbase.h` (lets the build work without Visual Studio's ATL component); `vulkan32/`, `vulkan32_fnv/`: the 32-bit `vulkan-1.lib` import-library stubs (the Vulkan SDK has no 32-bit one). Rever's own files, released under GPL-3.0 with the rest. |
| `REVER_README_*.md` | Rever's detailed notes for each build. |

## The shipped binaries and how each is built

| Shipped file (app 0.72) | SHA-256 | Arch | Patches, in order | Build helper |
| --- | --- | --- | --- | --- |
| `tools/opencomposite/vrclient_x64.dll` | `bdb2e5c5...` | x64 | `rever_opencomposite.patch`, `rever_opencomposite_gaze.patch`, `rever_opencomposite_alyx_depth.patch`, `rever_opencomposite_gestures.patch` | `atlbase.h` |
| `tools/opencomposite/vrclient.dll` | `6a187250...` | x86 | `rever_opencomposite.patch`, `rever_opencomposite_x86_alien.patch` (unchanged since 0.71) | `atlbase.h`, `vulkan32/` |
| `tools/opencomposite_fnv/vrclient.dll` | `c75c6b30...` | x86 | `rever_opencomposite.patch`, `rever_opencomposite_x86_alien.patch`, `rever_opencomposite_fnv.patch`, `rever_opencomposite_fnv_laser.patch`, `rever_opencomposite_fnv_keyboard.patch` | `atlbase.h`, `vulkan32_fnv/` |

`tools/opencomposite/steamvr_standin/vrmonitor.exe` is a separate Rever program (not OpenComposite, not linked to it)
and is not part of this source.

The x64 build of 0.72 (Visual Studio 2022, CMake, Vulkan SDK; details in `REVER_README_x64_x86.md` and
`REVER_README_GESTURES.md`), from the exact tree:

```bat
tar xzf source\OpenOVR-cff07db-rever-alyx-gestures-20261120-x64.tar.gz
cd OpenOVR
rem Vulkan SDK: copy Include and Lib\vulkan-1.lib into libs\vulkan\Include and libs\vulkan\Lib
set CL=/I"%CD%\..\build-helpers"
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DOC_VERSION=cff07db-rever-alyx-gestures-20261120
cmake --build build --config Release --target OCOVR
rem -> build\bin\Release\vrclient_x64.dll
```

or from the base and the patches: `tar xzf source\OpenOVR-cff07db-with-submodules.tar.gz`, `cd OpenOVR`,
`git init -q`, then `git apply` each x64 patch in the table's order (checked on 2026-11-25: the result is the exact
tree above, file for file, except the full `libs/openxr-sdk` checkout the build also has).

x86: build `vulkan-1.lib` from `build-helpers/vulkan32/` (or `vulkan32_fnv/` for New Vegas) as
`REVER_README_x64_x86.md` explains, copy it to `libs/vulkan/Lib32/`, then configure with `-A Win32` -> `vrclient.dll`.
The upstream tree has CRLF line endings and the x86 and New Vegas patches were written with LF, so apply those with GNU
patch: `patch -p1 --fuzz=3 < ..\patches\<name>.patch`, in the table's order (checked on 2026-11-25).

What each patch does:
- `rever_opencomposite_alyx_depth.patch` (0.72): with `REVER_ALYX_GAME_DEPTH=1` in the game's environment, the D3D11
  session hands Half-Life: Alyx's own depth to Rever's OpenXR layer through Rever's `srinputs` channel (new
  `DrvOpenXR/ReverGameDepth.cpp` / `.h`, with a desk test `ReverGameDepthTest.cpp`). Off unless set; other APIs and
  games unchanged.
- `rever_opencomposite_gestures.patch` (0.72): with `REVER_OC_GESTURES=1`, a hand whose interaction profile has a
  trigger touch answers `Partial` skeletal tracking, so Half-Life: Alyx shows the finger gestures estimated from the
  controller (`REVER_README_GESTURES.md`).
- `rever_opencomposite_fnv_keyboard.patch` (0.71.2): New Vegas' `ShowKeyboard` / `ShowKeyboardForOverlay` of the newer
  interfaces answer as the older ones (the placeholder that sends `KeyboardDone` at once) instead of aborting the game
  (`REVER_README_FNV.md`).

## How it relates to Rever VR Central

The app (closed source) never links OpenComposite. It copies `vrclient*.dll` into an OpenVR runtime folder and lists
that folder in `%LOCALAPPDATA%\openvr\openvrpaths.vrpath`; the game loads it. Rever's OpenXR layers and runtime are
reached by OpenComposite only through the standard OpenXR loader, plus documented hand-overs that appear in these
patches: a `rever_session_env.json` file read at start (environment variables), in the x64 gaze build the optional
`rever_streamer_gaze_dirs` export of Rever's runtime when that runtime is present, and in 0.72 the depth channel
`srinputs` above.

---

## Español

OpenComposite (GPL-3.0) traduce OpenVR a OpenXR. Rever VR Central 0.72 lleva tres builds con cambios de Rever; esta
carpeta es su código fuente correspondiente (sección 6 de la GPL-3.0). La release `0.71` cubría la 0.71.x; esta añade
lo de la 0.72: la profundidad de Half-Life: Alyx para la capa de Rever y los gestos de los dedos (x64), y el arreglo del
teclado de New Vegas (x86, incluido desde la 0.71.2).

- `source/`: el commit de upstream `cff07db` con sus submódulos (glm, OpenXR-SDK) y el árbol exacto del que sale el
  `vrclient_x64.dll` de la 0.72.
- `patches/`: los cambios de Rever, en el orden de la tabla de arriba según el binario.
- `build-helpers/`: `atlbase.h` y los stubs de `vulkan-1.lib` de 32 bits (archivos propios de Rever, también GPL-3.0).
- `REVER_README_*.md`: las notas de cada build.

| Binario de la app 0.72 | Parches, en orden |
| --- | --- |
| `tools/opencomposite/vrclient_x64.dll` (x64, `bdb2e5c5`) | `rever_opencomposite.patch`, `rever_opencomposite_gaze.patch`, `rever_opencomposite_alyx_depth.patch`, `rever_opencomposite_gestures.patch` |
| `tools/opencomposite/vrclient.dll` (x86, `6a187250`) | `rever_opencomposite.patch`, `rever_opencomposite_x86_alien.patch` |
| `tools/opencomposite_fnv/vrclient.dll` (x86, `c75c6b30`) | los dos anteriores + `rever_opencomposite_fnv.patch` + `rever_opencomposite_fnv_laser.patch` + `rever_opencomposite_fnv_keyboard.patch` |

La app (código cerrado) no enlaza OpenComposite: copia la DLL a una carpeta de runtime OpenVR y el juego la carga. Las
capas y el runtime de Rever solo se alcanzan por el cargador estándar de OpenXR y por los traspasos documentados de
arriba.
