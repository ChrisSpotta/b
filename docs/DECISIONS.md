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
