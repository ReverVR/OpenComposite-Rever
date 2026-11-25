# Rever's OpenComposite (GPL-3.0) - finger gestures (2026-11-20)

User's order: *"tenemos que activar en nuestra capa de openxr los gestos en los juegos, en Alyx estan desactivados"*.
KB ALX-005.

`rever_opencomposite_gestures.patch` goes on top of the tree `vrclient_x64.dll` 1b3abc1d was built from: `BASE_COMMIT` +
`rever_opencomposite.patch` + `rever_opencomposite_gaze.patch` + `rever_opencomposite_alyx_depth.patch` as of
2026-11-20 01:41 (its producer, without the later uncompiled vectors' fix). It changes nothing else.

- **Why:** Rever's OpenXR runtime has no `XR_EXT_hand_tracking`, so OpenComposite already estimates each hand's finger
  skeleton from the controller (`getEstimatedBoneData`: the index from the trigger's touch and pull, the thumb from any
  thumb touch, the other three fingers from the grip). It reported that skeleton as `VRSkeletalTracking_Estimated` for
  Oculus Touch controllers (a Vive wand's level). Half-Life: Alyx then keeps its own canned hands, closed by `HandCurl` /
  `TriggerPull`: no pointing, thumbs up, fist or open hand. SteamVR, and Rever's own SteamVR driver
  (`server_openvr/cpp/alvr_server/alvr_server.cpp`), report `Partial` for controllers that sense the fingers by touch.
- **What:** with `REVER_OC_GESTURES=1` in the game's environment, `GetSkeletalTrackingLevel` answers `Partial` for a hand
  whose interaction profile has a `/input/trigger/touch` input: Oculus Touch, which is how the Quest and the Steam Frame
  reach a game through Rever's runtime, and Index, already `Partial`. Real hand tracking still answers `Full`. Vive, WMR
  and the simple controller stay `Estimated`. Anything else than `1`: the levels exactly as before.
- **Header and test:** `OpenOVR/Reimpl/ReverGestures.h` is header-only, so `CMakeLists.txt` is unchanged.
  `OpenOVR/Reimpl/ReverGesturesTest.cpp` is a desk test with no game, no OpenXR and no GPU.
- **Log** (`%LOCALAPPDATA%\OpenComposite\logs\opencomposite.log`), each line once:
  - `Rever VR gestures: on (REVER_OC_GESTURES=1) ...` or `off (...)`;
  - `the game asks the left / right hand's skeletal level: Partial - finger curls from the controller's touch sensors
    (/interaction_profiles/oculus/touch_controller)`;
  - `the game reads the left hand's bones ...` and `... finger curls ...`;
  - each hand's first `'point'`, `'thumbs up'`, `'fist'`, `'open hand'`, `'finger gun'`.
- **Switch:** Rever's app writes `REVER_OC_GESTURES=1` into `<runtime>/rever_session_env.json` for the games in
  `rever_vr_mod_installer/opencomposite_gestures.py` `GAMES` (Half-Life: Alyx first). It does so only when the bundled
  DLL carries the switch's name. `REVER_ALYX_GESTURES=0` in the environment or in `rever_xr_runtime_tuning.ini` leaves it
  out.

## Build

The same as README.md's x64 build, from that tree, with VS 2022:

```
cmake -S <tree> -B <tree>\build -G "Visual Studio 17 2022" -A x64 -DOC_VERSION=cff07db-rever-alyx-gestures-20261120
cmake --build <tree>\build --config Release --target OCOVR
```

The desk test, in an x64 Native Tools prompt:

```
cl /nologo /EHsc /std:c++20 /W4 /DWIN32 /I<tree>\OpenOVR\Reimpl <tree>\OpenOVR\Reimpl\ReverGesturesTest.cpp
```

The x64 build of Rever VR Central 0.72 is the source archive's tree (`source/`), built exactly as above.
