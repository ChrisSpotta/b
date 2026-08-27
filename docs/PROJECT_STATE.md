# nw2s::b Project State

Updated: 2026-08-27

## Repository checkpoint

Branch: `modernisation-2026`

Last firmware/build-affecting commit before the project-documentation layer:

`3e8da6466bd9c985a78031fffa400f5f37301cee` — `Fix remaining JSON buffer null termination`

The modernisation history is intentionally linear and made of small, verified changes.

## Completed modernisation checkpoints

1. `7da381d` — Fix include filename case for Linux builds.
2. `77601db` — Make ARM toolchain path configurable and support ignored `Makefile.local`.
3. `a62e111` — Write linker map into `build/` instead of accidental tracked `.map`.
4. `9ace4cf` — Remove stale tracked linker-map artefact.
5. `5c8ac27` — Explicitly pin C to `gnu90` and C++ to `gnu++98`.
6. `02ae830` — Fix JSON configuration/program buffer null termination; hardware validated with GCC 6.3.1.
7. `3e8da64` — Fix the two remaining device-specific JSON buffer null-termination defects; hardware validated with GCC 6.3.1.

The initial behaviour-neutral build checkpoints above were tested with the GCC 4.8.3 Linux toolchain. Before intentional firmware-source behaviour changes, the resulting firmware retained the same SHA-256:

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

Known ordinary Linux GCC 4.8.3 runtime build SHA-256 before intentional firmware-source fixes:

`a4508c45e6960ae8db061a0732c3eaa8558ea32a200f55493a690f6e3a9f9d28`

The preserved historical image remains the primary rollback image.

## Local toolchains in WSL

Legacy reference compiler:

`~/nw2s-toolchains/gcc-arm-none-eabi-4.8.3-2014q1`

First newer compiler installed for experiment:

`~/nw2s-toolchains/gcc-arm-none-eabi-6-2017-q2-update`

Version:

`arm-none-eabi-gcc 6.3.1 20170620 (GNU Tools for ARM Embedded Processors 6-2017-q2-update)`

The GCC 6.3.1 toolchain has successfully built and linked the complete firmware, passed the documented static acceptance checks, and passed controlled validation on the actual nw2s::b hardware.

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

## GCC 6 compiler experiment — hardware validated

- GCC 6.3.1 successfully compiles and links the complete firmware.
- Two compatibility fixes were required:
  1. `utoa` parameter changed from `unsigned long` to `unsigned int` to match newer Newlib.
  2. `ArduinoSTLCompat.h` shields STL entry points from Arduino `min`/`max` macros.
- Both compatibility changes preserve the exact pre-source-fix GCC 4.8.3 reference binary:
  `a4508c45e6960ae8db061a0732c3eaa8558ea32a200f55493a690f6e3a9f9d28`
- GCC 6 compiler-validation firmware SHA-256 before the JSON source fix:
  `9a49756c2f647ef84db63e82f6a73de0ba09bf13dd4a749a6341de3ffb56c4e8`
- GCC 6 static comparison found:
  `.text`       197404
  `.ARM.exidx`     528
  `.relocate`     2808
  `.bss`          4320
- GCC 6 flash usage: 200740 bytes / 38.3%
- GCC 6 static SRAM usage: 7128 bytes / 7.3%
- Same prebuilt `libsam` archive is used by GCC 4.8.3 and GCC 6.
- `Reset_Handler` still comes from `startup_sam3xa.o` in that `libsam` archive.
- GCC 4.8.3 undefined symbols were investigated and found to be discarded-section references or expected weak runtime hooks.

### Hardware validation, 2026-08-25

The exact pre-validated GCC 6 candidate was staged separately and re-hashed on Windows before flashing:

`9a49756c2f647ef84db63e82f6a73de0ba09bf13dd4a749a6341de3ffb56c4e8`

It was uploaded manually through the Arduino Due Programming Port with Arduino BOSSA 1.6.1 using the standard erase/write/verify/boot/reset sequence. BOSSA wrote and successfully verified all 200740 bytes.

Observed hardware results:

- SAM3X booted normally after flashing.
- 19200-baud serial output was clean when captured as raw bytes.
- SD configuration JSON parsed successfully.
- Core IO initialisation completed.
- LED driver reported status `1`.
- `DEFAULT.B` parsed and the loader was reached.
- `PROG00` behaved as before, including VariableClock behaviour, LEDs and digital outputs.
- The test programs, including audio/SD-based programs, behaved as before.
- A complete power-off / cold-boot test passed, followed by successful `PROG00` and audio-program operation.

**Result:** GCC 6.3.1 hardware validation PASSED.

GCC 6 is the accepted newer-compiler baseline for the next modernisation work, while GCC 4.8.3 remains the legacy regression/reference compiler.

## Initial JSON buffer null-termination fix — hardware validated

Commit:

`02ae83091fc2dadaa0d01ac837736bac68ebd235` — `Fix JSON buffer null termination`

The SD configuration and program readers each allocate `fileSize + 1` bytes, read `fileSize` bytes, then pass the buffer to the aJSON parser. The old code used comparison instead of assignment at the final byte:

- `configData[fileSize] == '\0';`
- `programData[fileSize] == '\0';`

The commit changes only those two expressions to assignments so the parser input is explicitly null terminated.

Build verification:

- GCC 4.8.3 post-fix firmware SHA-256:
  `9d6ca61721f265bd60e82bc2a088699262dc770d3889afa9f7493043082cc742`
- GCC 4.8.3 post-fix size: 196288 bytes.
- GCC 6.3.1 post-fix firmware SHA-256:
  `90ff909c4cd2e8991c574a467d81b99e7b0b778c208108d1615956d5c5061684`
- GCC 6.3.1 post-fix size: 200748 bytes.
- GCC 6.3.1 post-fix sections:
  `.text`       197412
  `.ARM.exidx`     528
  `.relocate`     2808
  `.bss`          4320
- Only `b.cpp.o` and `SDFirmware.cpp.o` changed in the targeted comparison.
- Targeted disassembly showed one new byte-store of zero in each affected function.
- After rebasing the source commit onto the accepted GCC 6 documentation history, a fresh GCC 6 build reproduced the exact same `90ff909c...` firmware SHA-256 and 200748-byte size before push.

### Hardware validation, 2026-08-26

The exact GCC 6 post-fix image was separately staged and SHA-256 verified on Windows before flashing. Arduino BOSSA 1.6.1 wrote and successfully verified all 200748 bytes through the Due Programming Port.

Observed hardware results:

- Configuration JSON parsed successfully.
- Program JSON parsed successfully and the loader was reached.
- Existing archived configuration warnings about model/calibration format remained unchanged and are a separate compatibility issue.
- `PROG00` VariableClock/LED/digital-output behaviour passed.
- ByteBeat DAC1 audio passed.
- Known-good SD Looper playback passed.

**Result:** Initial JSON null-termination fix hardware validation PASSED.

That firmware SHA-256 was:

`90ff909c4cd2e8991c574a467d81b99e7b0b778c208108d1615956d5c5061684`

It is now superseded as the current development firmware by the remaining device-specific null-termination fix below.

## Remaining JSON buffer null-termination fixes — hardware validated

Commit:

`3e8da6466bd9c985a78031fffa400f5f37301cee` — `Fix remaining JSON buffer null termination`

Repository-wide review found the same comparison-versus-assignment defect in two device-specific SD configuration readers:

- `gcc/app/src/devices/GameOfLife.cpp` — `GameOfLife::readConfig()`
- `gcc/app/src/devices/BinaryArc.cpp` — `BinaryArc::readConfig()`

Both allocate `fileSize + 1`, read exactly `fileSize` bytes, then pass the buffer to aJSON. The commit changes only the two remaining `configData[fileSize] == '\0';` expressions to `configData[fileSize] = '\0';`. A repository search under `gcc/app/src/` found no remaining instances of this defect pattern after the change.

Build verification:

- GCC 4.8.3 firmware SHA-256:
  `c8cfd557ab5a9eaef1c151c337cfd070ed9a83e59d2a17779ec468833fd2a349`
- GCC 4.8.3 size: 196288 bytes.
- GCC 6.3.1 firmware SHA-256:
  `be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`
- GCC 6.3.1 size: 200748 bytes.
- GCC 6.3.1 sections remained:
  `.text`       197412
  `.ARM.exidx`     528
  `.relocate`     2808
  `.bss`          4320
- Only `GameOfLife.cpp.o` and `BinaryArc.cpp.o` changed in the targeted object comparison under both toolchains.
- Targeted disassembly showed one new `strb.w` zero-byte store after the SD-file read in each affected function.
- A fresh GCC 6.3.1 build from the clean committed tree reproduced the exact candidate SHA-256 `be8505d...` and 200748-byte size before flashing.

### Hardware validation, 2026-08-27

The exact GCC 6 image was copied from WSL to Windows and independently SHA-256 verified before flashing. Arduino BOSSA 1.6.1 found the SAM3X device, erased flash, wrote all 200748 bytes / 785 pages, verified all 200748 bytes successfully, set boot-from-flash and reset the CPU.

Serial startup after reset showed:

- Configuration parsed successfully.
- The existing `NW2S-B-1-1-0` model warning remained unchanged.
- The existing 16 output-calibration-format warnings remained unchanged.
- Core IO and LED-driver initialisation completed.
- Program JSON parsed successfully and the loader was reached.

The Due RESET button and the nw2s::b front-panel RESET button were both tested with the same 19200-baud serial capture and produced the same complete firmware restart sequence. For normal testing, the front-panel RESET can therefore be used as a practical processor reset.

Smoke-test results:

- `PROG00` VariableClock/LED/digital-output behaviour: PASS.
- ByteBeat DAC1 audio: PASS.
- Known-good SD Looper playback: PASS.

The preserved current SD-card archive contains only `sys.b` in its configuration directory, so the `GameOfLife::readConfig()` and `BinaryArc::readConfig()` file-loading paths were not separately runtime-exercised with dedicated `gamelife.cfg` / `binarc.cfg` files during this validation. Their source changes were statically verified by dual-toolchain build/object/disassembly comparison; the normal firmware image passed the established hardware smoke tests.

**Result:** Remaining JSON null-termination fix hardware validation PASSED.

The current validated GCC 6 development firmware is therefore:

`be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`

The older `90ff909c...` image is the validated state after the first two JSON null-termination fixes, and `9a49756c...` remains the pre-source-fix GCC 6 compiler-validation baseline. The GCC 4.8.3 `a4508c45...` image remains the pre-source-fix legacy regression reference.

## `getNotesFromJSON` missing-return fix — source/build verified

Commit:

`7505684e2562bc538afa7c8e6fd5ab1cf7886581` — `fix: return notes from getNotesFromJSON`

Adds a single `return notes;` line to `nw2s::getNotesFromJSON(aJsonObject*, const char*)` in `gcc/app/src/util/JSONUtil.cpp` after the existing `NoteSequenceData* notes = noteSequenceFromJSON(notesNode);` assignment. No header, no caller, no other source file is touched.

Build verification:

- GCC 4.8.3 firmware SHA-256:
  `c8cfd557ab5a9eaef1c151c337cfd070ed9a83e59d2a17779ec468833fd2a349`
- GCC 4.8.3 size: 196288 bytes (unchanged from its post-null-termination reference).
- GCC 6.3.1 firmware SHA-256:
  `be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`
- GCC 6.3.1 size: 200748 bytes.
- GCC 6.3.1 sections unchanged from the previous accepted firmware:
  `.text`       197412
  `.ARM.exidx`     528
  `.relocate`     2808
  `.bss`          4320
- Only `JSONUtil.cpp.o` differs in the targeted object comparison; the on-disk `.o` file differs only in DWARF debug line-number metadata (the source file is one line longer). The extracted `.text._ZN4nw2s16getNotesFromJSONEP11aJsonObjectPKc` and `.text._ZN4nw2s16getNotesFromJSONEP11aJsonObject` sections are **byte-identical** pre- and post-fix under both compilers, and the corresponding ELF disassembly shows the same machine code.
- Both pre-fix and post-fix GCC 4.8.3 `.bin` files compare byte-identical (`cmp` exit 0). Both pre-fix and post-fix GCC 6.3.1 `.bin` files compare byte-identical (`cmp` exit 0).
- Targeted one-off compile of the corrected `JSONUtil.cpp` with `-Wall -Wextra -Wreturn-type -Wmissing-noreturn` emits no `unused variable 'notes'` and no `control reaches end of non-void function` warnings on either compiler. GCC 6.3.1 with `-Werror=return-type` now builds `JSONUtil.cpp` successfully (exit 0).

### Acceptance

The corrected source produces the **same firmware bytes** as the image already validated on the actual nw2s::b hardware. No new firmware image was generated, no hardware reflash was performed, and no reflash is required — writing identical bytes to flash cannot provide any additional validation signal. The fix is accepted on the strength of:

- the source correction itself (single explicit `return notes;`);
- reproducible dual-toolchain build from a clean state;
- byte-for-byte `.bin` and per-symbol `.text` identity against the existing references;
- disappearance of the targeted `-Wreturn-type` / `-Wunused-variable` warning pair.

The current validated GCC 6 development firmware SHA-256 remains:

`be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`

and is also the post-`getNotesFromJSON`-correction firmware at the byte level.

## Build-tooling fixes

- `make clean` now removes both `build/` and `bin/`. `bin/` is recreated automatically via an order-only prerequisite on the `.bin` target, so a normal `make compile` still succeeds without any manual directory creation.
- Verified behaviour-neutral: rebuilt after the fix with both the GCC 4.8.3 and GCC 6.3.1 toolchains and confirmed the then-current reference SHA-256 hashes were unchanged.
- The duplicated `-Isrc/drivers/usbhost` include path has been removed from `INCLUDES`. GCC 4.8.3 still reproduced its exact pre-source-fix reference firmware hash, and GCC 6.3.1 still reproduced its exact accepted pre-source-fix firmware hash and section sizes, so the cleanup was behaviour-neutral.
- `-nostdlib` was confirmed to reach only `-c` compile commands and never the ELF link command, so it was removed from `COMMON_FLAGS` as an inert linker-only option. GCC 4.8.3 still reproduced its exact pre-source-fix reference firmware hash, and GCC 6.3.1 still reproduced its accepted pre-source-fix firmware hash and section sizes, so the cleanup was behaviour-neutral.
- The vendored SdFat packed FAT/MBR structures were reviewed under both GCC 4.8.3 and GCC 6.3.1. Structure sizes/offsets and representative generated access code were equivalent between the two compilers, and the reachable unaligned accesses in `SdVolume::init()` are supported by the Cortex-M3 target's normal unaligned-access behaviour. No source change is required.
- The stale `nodefaultlibs` explanation in `cxxabi-compat.cpp` was corrected to describe what the function actually does. The change is comment-only; GCC 4.8.3 and GCC 6.3.1 reproduced their exact then-current accepted firmware hashes and GCC 6 section sizes.

## Known later issues — not current work

These are recorded so they are not repeatedly rediscovered and should still be handled one at a time:

- The bundled prebuilt `libsam_sam3x8e_gcc_rel.a` contains GCC 4.5.2 object code, including the SAM startup/vector-table object. GCC 6 hardware validation passed while using this exact archive, so replacing/rebuilding it is not required merely to continue modernisation and must remain a separately tested decision.
- The legacy SD configuration format and parser disagree about module name/calibration structure in some archived configurations; preserve the working SD card and treat format/parser reconciliation as separate work.
- Two more `getNotesFromJSON`-style missing-return defects were discovered on 2026-08-27 in `gcc/app/src/devices/UsbMidi.cpp` and recorded in `docs/DECISIONS.md` for separate one-by-one investigation. Both functions currently fall off the end of non-void bodies and emit `no return statement in function returning non-void [-Wreturn-type]`:
  - `USBPolyphonicMidiController::create(aJsonObject* data)` at `UsbMidi.cpp:936-938`.
  - `USBMidiApeggiator::create(aJsonObject* data)` at `UsbMidi.cpp:1060-1063`.

  No source change, build, or commit has been made against either of these defects; the intended JSON contract for each factory must be investigated before any fix is attempted.
