# Rever's OpenComposite (GPL-3.0)

OpenComposite (https://gitlab.com/znixian/OpenOVR, branch `openxr`) translates OpenVR to OpenXR. Rever's
changes, first for Half-Life: Alyx (2026-10-07):

- OpenVR 2.15.6 headers: `IVRSystem_026`, `IVRCompositor_029`, `IVRInput_011`, `IVROverlay_028`,
  `IVRApplications_008` (the new methods answer "not available" where OpenXR has no equivalent).
- Valve's internal `IVRIPCResourceManagerClient_*` reported as not found instead of aborting the game.
- `scripts/libparse.py` understands `enum class X : type`.
- Half-Life: Alyx's loading screen ("Press Trigger to start", drawn by SteamVR) dismissed by itself:
  after the game's `show_message` on the `hlvr/interstitials` mailbox, `TriggerPull` (and
  `MenuDismiss`) read pressed for 250 ms every 2 s until `end_loading`.

- Rever's session settings: `<runtime>/rever_session_env.json` (`{"NAME": "value"}`, written by Rever's
  `opencomposite_bridge`) becomes the game's environment before OpenXR starts, so Rever's OpenXR layers
  load in a game Steam launches (which never inherits Rever's environment).
- A frame begun and never submitted (deferred first frame, `WaitGetPoses` twice) is ended empty before the next
  `xrWaitFrame`. Otherwise OFXR's frame generation takes the overlapping wait for a pipelined frame loop and
  refuses `xrBeginFrame` (`XR_ERROR_RUNTIME_FAILURE`, Alyx with Frame Generation, 2026-10-07).
- No SteamVR dashboard under OpenXR: `CreateDashboardOverlay` and the other dashboard calls refuse instead of
  aborting (ReShade's VR menu asks for one; Alyx with Rever's cinematic profile froze at start, 2026-10-07).

## Build (Windows, x64)

1. `git clone --branch openxr https://gitlab.com/znixian/OpenOVR.git` and check out `BASE_COMMIT`.
2. `git submodule update --init --depth 1 libs/glm libs/openxr-sdk`.
3. `git apply rever_opencomposite.patch`.
4. Copy the Vulkan SDK's `Include` and `Lib/vulkan-1.lib` into `libs/vulkan/Include` and `libs/vulkan/Lib`.
5. `cmake -S . -B build -G "Visual Studio 18 2026" -A x64`
6. Without Visual Studio's ATL component: `set CL=/I"<this folder>"` (uses `atlbase.h` here).
7. `cmake --build build --config Release --target OCOVR` -> `build/bin/Release/vrclient_x64.dll`.

Used as an OpenVR runtime: `<runtime>/bin/vrclient_x64.dll`, `<runtime>` first in
`%LOCALAPPDATA%\openvr\openvrpaths.vrpath`'s `runtime` list (Half-Life: Alyx's own `openvr_api.dll` is
Valve's internal build and cannot be replaced).

## Build (Windows, x86) - 32-bit games (Alien Isolation, 2026-10-08)

The same source and patch, built for Win32: `<runtime>/bin/vrclient.dll` next to `vrclient_x64.dll`.

1. The Vulkan SDK ships no 32-bit `vulkan-1.lib`. Build the import library from the stub in `vulkan32/`
   (the 20 Vulkan functions OpenComposite calls, `__stdcall`, exported undecorated as the real `vulkan-1.dll`):
   `cl -nologo -c -O1 -GS- stub.c` then
   `link -nologo -dll -noentry -nodefaultlib -machine:x86 -def:vulkan-1.def -out:vulkan-1.dll stub.obj`
   (x86 tools: `VC\Tools\MSVC\<ver>\bin\Hostx64\x86`), and copy `vulkan-1.lib` to `libs/vulkan/Lib32/`.
   (A `lib /def` import library with `name@N` entries imports the decorated names and the DLL fails to load.)
2. `cmake -S . -B build32 -G "Visual Studio 18 2026" -A Win32`
3. `set CL=/I"<this folder>"`, then `cmake --build build32 --config Release --target OCOVR`
   -> `build32/bin/Release/vrclient.dll` (check with `dumpbin -imports`: `vulkan-1.dll` names undecorated).

The x86 build alone also carries `rever_opencomposite_x86_alien.patch`, applied on top of
`rever_opencomposite.patch` (Alien Isolation's MotherVR, headset-verified 2026-10-09). With it:
- eyes submitted with bounds that leave nothing show no scene, only the overlays, as SteamVR does;
- overlays get their whole transform, an absolute one in the game's pose space;
- a screen straight ahead of the seated origin is anchored to the tracked headset;
- a headset-relative overlay goes in the view space.

Never rebuild or copy `vrclient_x64.dll` with it: the 64-bit build is under Half-Life: Alyx's freeze.
