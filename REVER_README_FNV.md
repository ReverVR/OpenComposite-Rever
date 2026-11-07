# Rever's OpenComposite (GPL-3.0) - Fallout: New Vegas (FNVVR Reborn, 2026-10-20)

`rever_opencomposite_fnv.patch` on top of the x86 build above -> `rever_vr_mod_installer/tools/opencomposite_fnv/vrclient.dll`,
copied over the runtime's `vrclient.dll` only for New Vegas' session (`fallout_nv_vr_options.use_fnv_opencomposite`).
The frozen `tools/opencomposite/vrclient.dll` (Alien Isolation) is never rebuilt from it. With it:
- `IVRInput_003` (OpenVR 1.0.15), with its 24-byte `VRActiveActionSet_t` copied into the current struct;
- `SetOverlayRaw` drawn (FNVVR's amber laser pointers, an 8x8 RGBA image; `rever_opencomposite_fnv_laser.patch`, 2026-10-21):
  the pixels are kept premultiplied and uploaded at the next eye submission (the game sets them outside DXVK's submission
  lock, so its Vulkan queue is only ours inside its frame), into an R8G8B8A8_SRGB swapchain through a host-visible staging
  buffer (`VkCompositor::ReverCopyRaw`), with the device of the last Vulkan texture the game handed over; such an overlay is
  exempt from the <=8x8 skip, and every overlay's height now follows its texel aspect as SteamVR's (the laser's length);
- `GetCumulativeStats` answers empty stats for this process;
- a game that updates an overlay with no `WaitGetPoses` for 200 ms (FNVVR's intro movie on its panel) gets frames with
  only the overlays, as SteamVR composites them by itself.
If `stubs.gen.cpp` keeps a method implemented by hand, run `python scripts/stubs.py OpenOVR/Reimpl OpenVRHeaders build32/generated`.

`rever_opencomposite_fnv_laser.patch` goes on top of `rever_opencomposite_fnv.patch`. It needs 11 more Vulkan functions in
the 32-bit import library: build it from `vulkan32_fnv/` as `vulkan32/` says (`vulkan32/` stays Alien Isolation's, frozen).
