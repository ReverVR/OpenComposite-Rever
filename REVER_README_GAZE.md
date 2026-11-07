# Rever's OpenComposite (GPL-3.0) - eye-tracked foveation (2026-10-28)

`rever_opencomposite_gaze.patch`, applied after `rever_opencomposite.patch` (README.md's x64 build, step 3), gives
`rever_vr_mod_installer/tools/opencomposite/vrclient_x64.dll`:

- `GetEyeTrackedFoveationCenter` / `...ForProjection` answer the Steam Frame's gaze from Rever's runtime in the game's
  process (`rever_xr_runtime.dll`'s `rever_streamer_gaze_dirs`), for Half-Life: Alyx's foveated rendering; without it,
  no eye tracking as before.
- `#include <chrono>` in `XrHMD.cpp` (the current compiler needs it).

Build the x64 DLL from a clean tree (`git worktree add` at `BASE_COMMIT`): the working tree in `native/opencomposite_src`
also carries the x86 / New Vegas patches.
