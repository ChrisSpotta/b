# nw2s::b Project State

Updated: 2026-08-25

## Repository checkpoint

Branch: `modernisation-2026`

Last firmware/build-affecting commit before the project-documentation layer:

`5c8ac2724fec66b175334de7415d44d22b2cc413` — `Pin legacy C and C++ language standards`

The modernisation history is intentionally linear and made of small, verified changes.

## Completed modernisation checkpoints

1. `7da381d` — Fix include filename case for Linux builds.
2. `77601db` — Make ARM toolchain path configurable and support ignored `Makefile.local`.
3. `a62e111` — Write linker map into `build/` instead of accidental tracked `.map`.
4. `9ace4cf` — Remove stale tracked linker-map artefact.
5. `5c8ac27` — Explicitly pin C to `gnu90` and C++ to `gnu++98`.

Each build-affecting checkpoint above was tested with the GCC 4.8.3 Linux toolchain. The resulting firmware retained the same SHA-256:

`a4508c45e6960ae8db061a0732c3eaa8558ea32a200f55493a690f6e3a9f9d28`

## Reproducible legacy baseline

Target:

- Arduino Due / ATSAM3X8E / Cortex-M3
- Arduino SAM core 1.5.6
- CMSIS 2.10
- bundled prebuilt `libsam_sam3x8e_gcc_rel.a`

Legacy compiler:

`arm-none-eabi-gcc 4.8.3 20140228`

The project/core object code was reproduced exactly under WSL with the matching Linux-hosted GCC 4.8.3 compiler. Relinking those reproduced objects with the preserved 2017 ARM target runtime/start files yielded an exact historical flash image.

Known historical flash-image SHA-256:

`9ad907b8fa3f107ffea77dd68ea7e0aa05f70f6ce50a73711c329aede3cab26e`

Known ordinary Linux GCC 4.8.3 runtime build SHA-256:

`a4508c45e6960ae8db061a0732c3eaa8558ea32a200f55493a690f6e3a9f9d28`

No modernisation firmware has been flashed to hardware.

## Local toolchains in WSL

Legacy reference compiler:

`~/nw2s-toolchains/gcc-arm-none-eabi-4.8.3-2014q1`

First newer compiler installed for experiment:

`~/nw2s-toolchains/gcc-arm-none-eabi-6-2017-q2-update`

Version:

`arm-none-eabi-gcc 6.3.1 20170620 (GNU Tools for ARM Embedded Processors 6-2017-q2-update)`

The GCC 6 toolchain has been installed and its executable verified, but it has not yet been used to build this repository at the time of this checkpoint.

## Current build configuration

`gcc/app/Makefile` now supports a machine-local ignored file:

`gcc/app/Makefile.local`

The current WSL machine uses that file for the GCC 4.8.3 toolchain path during normal legacy-reference builds.

The tracked build explicitly pins:

- C: `-std=gnu90`
- C++: `-std=gnu++98`

This prevents a future compiler swap from silently changing the source-language dialect at the same time.

Linker maps are generated as:

`gcc/app/build/nw2s-b-1.2-sd.map`

## Current experiment

Next step: build the completely unchanged source/core/platform with GCC 6.3.1 by overriding only `TOOLCHAIN_PATH` on the command line.

From `gcc/app`:

```sh
make clean
make \
  TOOLCHAIN_PATH="$HOME/nw2s-toolchains/gcc-arm-none-eabi-6-2017-q2-update/bin" \
  compile
```

For the first attempt:

- do not edit firmware source;
- do not rebuild or replace `libsam`;
- do not update Arduino SAM/CMSIS;
- do not change compile or link flags;
- do not fix errors before recording the first failure;
- do not upload or flash firmware.

If the GCC 6 build succeeds, compare it with the GCC 4.8.3 baseline using binary size, section sizes, symbol set, linker map and targeted disassembly. A matching SHA-256 is not expected from a different compiler/runtime package.

## Known later issues — not current work

These are recorded so they are not repeatedly rediscovered, but they should not be mixed into the GCC 6 experiment:

- `-nostdlib` appears in compile-only flags where it has no practical effect.
- `-Isrc/drivers/usbhost` is duplicated in the include list.
- `make clean` removes `build/` but not `bin/`, so stale `.bin` files can survive a clean.
- `cxxabi-compat.cpp` contains an old comment about `nodefaultlibs` that no longer matches the actual link command.
- Packed FAT/MBR structures are worth targeted review under newer compilers.
- The bundled prebuilt `libsam_sam3x8e_gcc_rel.a` contains GCC 4.5.2 object code, including the SAM startup/vector-table object; leave it untouched for the first compiler experiment.
