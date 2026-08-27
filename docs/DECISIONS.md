# nw2s::b Technical Decisions

This file records durable project decisions and their rationale. It is not a changelog; Git history remains the authoritative record of changes.

## 2026-08-21 — Preserve a known-good baseline before modernising

**Decision:** Preserve the historical source, firmware binaries and SD-card contents before changing or flashing anything.

**Reason:** The module is old, personally built, and the original development environment is obsolete. Modernisation must remain reversible and must not destroy the last known-good state.

**Consequence:** Modernisation work happens in a separate Git branch/workspace. No firmware is flashed merely because it compiles.

## 2026-08-24 — Treat exact flash-image reproduction as Stage 1 completion

**Decision:** Accept the reproduced `.bin` flash image as the relevant byte-identical legacy result rather than pursuing byte-identical ELF metadata.

**Evidence:** Newly reproduced project/core objects were identical to the historical objects. Relinking them with the preserved 2017 ARM target runtime/start files produced the exact historical flash-image SHA-256:

`9ad907b8fa3f107ffea77dd68ea7e0aa05f70f6ce50a73711c329aede3cab26e`

**Reason:** The `.bin` is the image that would actually be flashed. ELF/debug/container differences are not required for firmware identity.

## 2026-08-24 — Modernise in small behaviour-neutral commits

**Decision:** Keep each infrastructure change independent and verify the legacy GCC 4.8.3 Linux-runtime firmware hash after every build-affecting change.

Reference SHA-256:

`a4508c45e6960ae8db061a0732c3eaa8558ea32a200f55493a690f6e3a9f9d28`

**Reason:** Small commits make regressions easy to identify and revert, and avoid mixing build-system changes with firmware behaviour changes.

## 2026-08-24 — Preserve historical file formatting where practical

**Decision:** Do not broadly normalise line endings or reformat old source files during targeted fixes.

**Reason:** The repository contains mixed historical line endings. A previous text-mode edit demonstrated how a tiny semantic change could accidentally become a very large formatting diff.

## 2026-08-24 — Make the ARM toolchain location configurable

**Decision:** Keep the historical in-tree toolchain path as the Makefile default, but allow `TOOLCHAIN_PATH` / `TOOLCHAIN_PREFIX` overrides and an ignored machine-local `gcc/app/Makefile.local`.

**Reason:** This keeps the repository portable while avoiding shell-profile or machine-specific paths in tracked source.

## 2026-08-24 — Keep linker maps, but generate them under build/

**Decision:** Replace the accidental `$(NEWMAINFILE).map` output with `$(TMPDIR)/$(PROJNAME).map` and remove the stale tracked `.map` artefact separately.

**Reason:** `NEWMAINFILE` had never been defined, so the linker wrote a file literally named `.map`. Map files are useful diagnostics, but they are generated build artefacts and belong under the ignored build directory.

## 2026-08-24 — Pin the legacy language dialect before changing compilers

**Decision:** Explicitly compile C as `gnu90` and C++ as `gnu++98`.

**Reason:** GCC defaults changed in later releases. Without explicit `-std=` flags, changing compiler would also silently change the source-language dialect, confounding a single-variable toolchain experiment.

**Evidence:** Adding the explicit standards under GCC 4.8.3 retained the exact Linux-runtime firmware hash.

## 2026-08-25 — Keep Arduino SAM 1.5.6 and prebuilt libsam for the first compiler experiment

**Decision:** For the first newer-compiler build, change only the GNU Arm toolchain. Keep the existing Arduino SAM core, CMSIS, source and `libsam_sam3x8e_gcc_rel.a` unchanged.

**Reason:** The prebuilt libsam is plain-C ARM EABI code and has historically already been linked across a GCC-version gap. Rebuilding libsam or replacing the SAM platform at the same time would introduce additional major variables, including startup/vector-table code.

**Consequence:** A compiler-only build failure is useful evidence and should be recorded before attempting any source or platform fix.

## 2026-08-25 — Use GCC 6.3.1 as the first newer compiler experiment

**Decision:** Use GNU Tools for ARM Embedded Processors `6-2017-q2-update` (GCC 6.3.1) as the first newer toolchain experiment rather than GCC 4.9.

**Reason:** GCC 6-2017-q2 provides a native 64-bit Linux host package suitable for WSL. The language standards are now explicitly pinned to `gnu90` / `gnu++98`, so the compiler package can be changed without silently changing those dialects.

**Constraint:** Do not make GCC 6 the normal project toolchain yet. Invoke it by overriding `TOOLCHAIN_PATH` for the experiment.

## 2026-08-25 — Do not use binary identity as the acceptance test for a newer compiler

**Decision:** When a newer compiler is introduced, judge the experiment using compile/link success, binary and section sizes, symbol set, linker map, warnings, targeted disassembly and ultimately controlled hardware behaviour — not matching the GCC 4.8.3 SHA-256.

**Reason:** Different compiler/runtime packages supply different libgcc/newlib/C++ support/start files and will normally produce different machine code even from unchanged source.

## 2026-08-25 — Use lightweight agent-neutral project memory

**Decision:** Coordinate Sonnet, Codex, MiniMax, Google or other coding agents through `AGENTS.md`, `docs/PROJECT_STATE.md` and this file rather than installing a large project-specific AI orchestration system.

**Reason:** Git already provides the real history. A small shared context layer gives different agents the same constraints, current checkpoint and rationale without adding another complex workflow to maintain.

**Working rule:** One agent edits the working tree at a time; other agents may review. Meaningful new state belongs in `PROJECT_STATE.md`; durable architectural decisions belong here.

## 2026-08-25 — GCC 6.3.1 passed static acceptance gate

**Decision:** GCC 6.3.1 passed the first compiler-modernisation static acceptance gate. Further compiler upgrades are paused until GCC 6 has been validated on the actual nw2s::b hardware.

**Reason:** Acceptance was based on successful complete build/link, preserved legacy binary identity with GCC 4.8.3 after compatibility changes, startup/HAL verification, linker-map/runtime inspection, symbol accounting and reasonable memory usage. Binary identity between GCC 4.8.3 and GCC 6 is not required.

## 2026-08-25 — Retain vendored SdFat packed FAT/MBR parsing code unchanged

**Decision:** Retain the existing vendored SdFat packed FAT/MBR parsing code unchanged for the current SAM3X8E target.

**Reason:** Targeted comparison showed identical structure layouts and materially equivalent packed-member access code under GCC 4.8.3 and GCC 6.3.1. The reachable unaligned accesses rely on ARMv7-M behaviour supported by the Cortex-M3 target, and no compiler migration defect was found.

**Consequence:** Do not rewrite these structures merely as compiler-modernisation cleanup. Revisit only if moving to a target/alignment model where those assumptions no longer hold, and treat that as a separately tested change.

## 2026-08-25 — Accept GCC 6.3.1 after hardware validation

**Decision:** Accept GCC 6.3.1 as the validated newer-compiler baseline for continuing nw2s::b modernisation. Keep GCC 4.8.3 as the exact-hash regression reference, and do not advance to another compiler merely because GCC 6 validation is complete.

**Evidence:** The exact GCC 6 firmware image with SHA-256 `9a49756c2f647ef84db63e82f6a73de0ba09bf13dd4a749a6341de3ffb56c4e8` was flashed through the Arduino Due Programming Port using Arduino BOSSA 1.6.1. BOSSA wrote and verified 200740 bytes successfully. The firmware then booted normally, produced clean 19200-baud serial output, parsed the SD configuration and default program, initialised the IO and LED driver, reached the loader, ran `PROG00` as before, ran the test programs including audio/SD-based programs, and passed a complete power-off/cold-boot retest.

**Reason:** The newer compiler has now passed both the static acceptance gate and real-hardware behavioural validation without requiring a SAM core or `libsam` change.

**Consequence:** GCC 6 may be used for the next modernisation work. The Makefile/toolchain default should not be changed as part of this decision; making GCC 6 the normal/default build remains a separate, testable change. Rebuilding or replacing the old prebuilt `libsam` also remains separate work rather than an automatic next step.

## 2026-08-26 — Fix and accept explicit JSON buffer null termination

**Decision:** Replace the two erroneous null-termination comparisons with assignments in the SD configuration and program readers, and accept the change after targeted build analysis and real-hardware validation.

**Change:** In `gcc/app/src/util/b.cpp` and `gcc/app/src/util/SDFirmware.cpp`, change `buffer[fileSize] == '\0';` to `buffer[fileSize] = '\0';`. The final rebased source commit is `02ae83091fc2dadaa0d01ac837736bac68ebd235` (`Fix JSON buffer null termination`).

**Evidence:** The affected buffers are allocated as `fileSize + 1`, populated with exactly `fileSize` bytes and then passed to aJSON, so the old comparison left the parser input without an explicit terminator. GCC 6.3.1 produced firmware SHA-256 `90ff909c4cd2e8991c574a467d81b99e7b0b778c208108d1615956d5c5061684`, size 200748 bytes. Targeted comparison showed only the two affected source objects changing and one added zero-byte store in each affected function. After the commit was rebased onto the accepted GCC 6 documentation history, a fresh build reproduced the exact same firmware hash and size.

The exact post-fix image was staged and SHA-256 verified on Windows, then flashed through the Arduino Due Programming Port with Arduino BOSSA 1.6.1. BOSSA wrote and verified all 200748 bytes successfully. Serial startup showed successful configuration and program parsing and reached the loader. `PROG00`, ByteBeat DAC1 audio and known-good SD Looper playback all passed on the actual module. Existing archived configuration warnings about module/calibration format were unchanged and remain a separate issue.

**Reason:** The original code relied on undefined buffer contents beyond the bytes read from the SD file. Explicitly terminating the C strings is the smallest correct fix and removes that parser-input hazard without combining it with unrelated SD-format compatibility work.

**Consequence:** The current validated GCC 6 development firmware SHA-256 is `90ff909c4cd2e8991c574a467d81b99e7b0b778c208108d1615956d5c5061684`. The earlier `9a49756c...` GCC 6 image remains the pre-source-fix compiler-validation baseline. The GCC 4.8.3 `a4508c45...` hash remains the pre-source-fix legacy regression reference; the intentional source fix has its own GCC 4.8.3 build hash `9d6ca61721f265bd60e82bc2a088699262dc770d3889afa9f7493043082cc742`. Continue to treat SD configuration model/calibration reconciliation as separate work.

## 2026-08-27 — Complete and accept JSON null termination in device-specific loaders

**Decision:** Fix the two remaining instances of the same null-termination comparison bug in the device-specific Game of Life and Binary Arc configuration readers, and accept the change after dual-toolchain static verification and controlled hardware validation.

**Change:** Commit `3e8da6466bd9c985a78031fffa400f5f37301cee` (`Fix remaining JSON buffer null termination`) changes only `GameOfLife::readConfig()` and `BinaryArc::readConfig()`, replacing `configData[fileSize] == '\0';` with `configData[fileSize] = '\0';`. A repository-wide search under `gcc/app/src/` found no remaining occurrences of that defect pattern after the change.

**Evidence:** Both readers allocate `fileSize + 1`, read exactly `fileSize` bytes and pass the buffer to aJSON, so the old comparison left the parser input unterminated. GCC 4.8.3 produced firmware SHA-256 `c8cfd557ab5a9eaef1c151c337cfd070ed9a83e59d2a17779ec468833fd2a349`, size 196288 bytes. GCC 6.3.1 produced firmware SHA-256 `be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`, size 200748 bytes. Only `GameOfLife.cpp.o` and `BinaryArc.cpp.o` changed in the targeted object comparisons, and disassembly showed the expected new zero-byte store in each function. A fresh GCC 6.3.1 build from the clean committed tree reproduced the same `be8505d...` image byte-for-byte before flashing.

The exact GCC 6 image was copied to Windows and independently SHA-256 verified, then flashed through the Arduino Due Programming Port with Arduino BOSSA 1.6.1. BOSSA wrote and verified all 200748 bytes / 785 pages successfully, set boot-from-flash and reset the CPU. Serial startup parsed configuration and program JSON successfully, completed IO/LED initialisation and reached the loader; the existing archived module/calibration-format warnings were unchanged. `PROG00`, ByteBeat DAC1 audio and known-good SD Looper playback all passed. The Due RESET and nw2s::b front-panel RESET were also shown to produce the same complete firmware restart sequence under serial capture.

The preserved current SD-card archive does not contain dedicated `gamelife.cfg` or `binarc.cfg` files, so those two device-specific file-loading paths were not separately runtime-exercised during this validation. Their changes were verified statically under both compilers, while the normal firmware image passed the established hardware smoke-test set.

**Reason:** These are the same clear correctness defect already accepted in the general configuration/program loaders. Completing the fix removes the remaining known instances without combining unrelated refactoring or SD-format compatibility work.

**Consequence:** The current validated GCC 6 development firmware SHA-256 is now `be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`. The earlier `90ff909c...` image remains the validated checkpoint after the first two JSON null-termination fixes; `9a49756c...` remains the pre-source-fix GCC 6 compiler-validation baseline. Continue to handle further defects one at a time, and keep SD model/calibration-format reconciliation separate.

## 2026-08-27 — Return value from `getNotesFromJSON`

**Decision:** Add an explicit `return notes;` to `nw2s::getNotesFromJSON(aJsonObject*, const char*)` in `gcc/app/src/util/JSONUtil.cpp`, accept the change purely on source/build/static verification, and record two further instances of the same defect class in `UsbMidi.cpp` as separate outstanding defects rather than fixing them now.

**Change:** Commit `7505684e2562bc538afa7c8e6fd5ab1cf7886581` (`fix: return notes from getNotesFromJSON`) adds exactly one line — `return notes;` — after the existing `NoteSequenceData* notes = noteSequenceFromJSON(notesNode);` assignment. The preceding assignment is intentionally left unchanged. No header, no caller, no other source file, no Makefile, no flag, no documentation is touched by that commit.

**Defect:** The 2-argument `getNotesFromJSON` is declared `NoteSequenceData*` and is non-void, but its body originally fell off the end after assigning the result of `noteSequenceFromJSON(notesNode)` to a local `notes` variable and never returning it. Per C++98 §6.6.3/2 (and the same wording in later standards, with `main` as the only special case), falling off the end of a value-returning function is undefined behaviour. The local variable was also flagged as unused by `-Wunused-variable`. A targeted one-off compile of `JSONUtil.cpp` with `-Wall -Wextra -Wreturn-type -Wmissing-noreturn` produced both warnings on both compilers before the fix; GCC 6.3.1 escalated the missing-return diagnostic to an error under `-Werror=return-type`. After the fix, both warnings are gone and GCC 6.3.1 with `-Werror=return-type` builds `JSONUtil.cpp` successfully.

**Evidence:** The fix is the smallest possible semantic change. It deliberately does **not** refactor the assignment into `return noteSequenceFromJSON(notesNode);`, because the investigation and this decision both treat the existing assignment-and-return pair as the canonical source-level intent and want to keep the per-line diff minimal and reviewable.

Built from a clean state with both accepted toolchains after the source commit:

- GCC 4.8.3 firmware SHA-256: `c8cfd557ab5a9eaef1c151c337cfd070ed9a83e59d2a17779ec468833fd2a349` (196288 bytes).
- GCC 6.3.1 firmware SHA-256: `be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0` (200748 bytes).

Both hashes are **byte-for-byte identical** to their respective pre-fix references recorded in `docs/PROJECT_STATE.md` (the GCC 4.8.3 hash predates this commit and is the post-`c8cfd557…` reference for that compiler; the GCC 6.3.1 hash is the currently validated hardware image). The `.text`, `.ARM.exidx`, `.relocate` and `.bss` section sizes are unchanged on both compilers. Per-object comparison of `JSONUtil.cpp.o` against the pre-fix baseline shows the on-disk `.o` file differs only in DWARF debug line-number metadata (the source file is one line longer), while the `.text._ZN4nw2s16getNotesFromJSONEP11aJsonObjectPKc` and `.text._ZN4nw2s16getNotesFromJSONEP11aJsonObject` section extracts are **byte-identical** pre- and post-fix. Targeted disassembly comparison on both compilers confirms the generated machine code for both `getNotesFromJSON` overloads is unchanged: the success path already relied on the natural ARM AAPCS r0-as-return-register encoding after the `noteSequenceFromJSON()` call, so the explicit `return notes;` does not introduce a new instruction. The production Makefile uses `-w`, so the build log itself reports zero warnings either way.

**Reason:** The C++ source was technically undefined behaviour. Even though both compilers happened to encode the success path in a way that returns the correct pointer in practice, no compiler is obligated to preserve that behaviour, and a future compiler version, optimisation flag change, or inlining decision is allowed to break it. Adding the single explicit `return notes;` removes the UB without changing observable behaviour or the emitted firmware.

**Consequence:** The GCC 6.3.1 firmware SHA-256 remains `be8505d77e295c6fd39e9c01c526748374c56b4dcdcc4414785f3f89e42cbce0`. The currently validated hardware image is therefore also the post-fix firmware at the byte level, and **no hardware reflash was performed or is required** — flashing the same bytes again cannot provide any additional validation signal. Acceptance of this defect fix rests on reproducible source correction, dual-toolchain build verification, byte-for-byte firmware comparison against the existing references, per-symbol code comparison, and the absence of the targeted return-type warning. Continue to treat any remaining source defects as separate one-by-one work.

## 2026-08-27 — Known outstanding missing-return defects in `UsbMidi.cpp`

**Decision:** Record, but do not fix in this cycle, two further instances of the same `getNotesFromJSON`-style missing-return defect discovered while reviewing the JSON-related source on 2026-08-27.

**Defects:**

- `nw2s::USBPolyphonicMidiController* nw2s::USBPolyphonicMidiController::create(aJsonObject* data)` at `gcc/app/src/devices/UsbMidi.cpp:936-938`. The body is empty `{}` and currently falls off the end of a non-void function. Called from `gcc/app/src/util/SDFirmware.cpp` when parsing an SD program whose JSON device entry has `"type": "USBPolyphonicMidiController"`.
- `nw2s::USBMidiApeggiator* nw2s::USBMidiApeggiator::create(aJsonObject* data)` at `gcc/app/src/devices/UsbMidi.cpp:1060-1063`. The body is `{ }` and currently falls off the end of a non-void function. Called from `gcc/app/src/util/SDFirmware.cpp` for `"type": "USBMidiApeggiator"`.

Both functions emit `warning: no return statement in function returning non-void [-Wreturn-type]` on the unmodified source under both GCC 4.8.3 and GCC 6.3.1 when compiled with `-Wall -Wextra -Wreturn-type`.

**Status:** Recorded only. **No source change, build, or commit has been made against either of these defects.** They are intentionally left for a separate future investigation cycle.

**Reason for not fixing them in this cycle:** These are factory functions that should consume the supplied `aJsonObject* data`, parse it, and return a fully configured object. The investigation needed to determine what each one should do — i.e. what fields it needs to read from the JSON and which constructor to invoke — was deliberately not performed here. Speculating about the intended JSON shape or copying the structure of another working factory without verifying the original design intent could introduce a new behaviour change that would belong in its own reviewed commit and would deserve its own build verification and, if it changes emitted code, its own hardware acceptance pass.

**Consequence:** These two defects remain in the tree as known outstanding issues. They are recorded here so they are not repeatedly rediscovered. They must be handled in a separate one-by-one defect cycle, starting with an investigation-only pass on the intended JSON contract of each factory before any code change is proposed.
