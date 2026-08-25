# Agent Workflow Guidance

This repository is being modernised cautiously. Preserve behaviour first; improve infrastructure and code only in small, reviewable steps.

## Read before working

Before making changes, read:

1. `AGENTS.md`
2. `docs/PROJECT_STATE.md`
3. `docs/DECISIONS.md`

Treat those files, the Git history, and reproducible build results as the project source of truth. Do not infer project state from an earlier chat transcript when the repository says otherwise.

## Working rules

- Work on `modernisation-2026` unless explicitly told otherwise.
- Inspect first. Make only the requested change.
- Keep commits small and single-purpose.
- Do not combine cleanup, refactoring, dependency updates, compiler changes, and firmware changes in one step.
- Do not normalise line endings or rewrite whole legacy files merely to change a few tokens. This repository contains historical mixed line endings.
- Do not run broad automatic formatters over legacy source.
- Check `git status` before and after work. Do not overwrite unrelated local changes.
- One agent should edit the working tree at a time. Other agents may review, but avoid simultaneous edits by multiple agents.
- Do not commit or push unless explicitly requested.
- If an experiment fails, preserve and report the first failure before attempting fixes.

## Hardware safety

- Never run `make upload`, `bossac`, or any other flashing/upload command unless the user explicitly authorises flashing in the current task.
- Do not erase the Arduino Due.
- Firmware build experiments are not permission to put firmware on hardware.

## Legacy build baseline

Target hardware:

- Arduino Due / ATSAM3X8E
- Cortex-M3, 84 MHz
- Arduino SAM core 1.5.6
- CMSIS 2.10
- bundled prebuilt `libsam_sam3x8e_gcc_rel.a`

Reference legacy compiler:

- GNU Arm Embedded GCC 4.8.3-2014q1

Reference Linux-runtime firmware SHA-256:

`a4508c45e6960ae8db061a0732c3eaa8558ea32a200f55493a690f6e3a9f9d28`

Known exact historical flash-image SHA-256 when relinked with the preserved 2017 target runtime/start files:

`9ad907b8fa3f107ffea77dd68ea7e0aa05f70f6ce50a73711c329aede3cab26e`

A behaviour-neutral change built with the legacy Linux toolchain should normally retain the Linux-runtime reference hash unless there is a documented reason it cannot.

## Normal legacy build

From `gcc/app`:

```sh
make clean
make compile
sha256sum bin/nw2s-b-1.2-sd.bin
```

The machine-specific toolchain location belongs in the ignored `gcc/app/Makefile.local`, not in tracked source.

## Current compiler-modernisation discipline

The C and C++ dialects are explicitly pinned to the legacy defaults:

- C: `gnu90`
- C++: `gnu++98`

For the first newer-compiler experiments:

- keep firmware source unchanged;
- keep Arduino SAM 1.5.6 unchanged;
- keep the prebuilt `libsam_sam3x8e_gcc_rel.a` unchanged;
- change only `TOOLCHAIN_PATH` on the command line;
- do not expect a newer compiler to produce the legacy SHA-256;
- compare build success, binary/section sizes, symbols, linker map, warnings, and targeted disassembly before considering hardware testing.

Do not fix compiler errors until the original failure has been recorded and understood.

## Project documentation

`docs/PROJECT_STATE.md` is a concise current checkpoint, not a diary.

`docs/DECISIONS.md` records durable technical decisions and why they were made.

When a meaningful project stage changes, keep those documents accurate. Do not rewrite past decisions to make history look cleaner; append a superseding decision when necessary.
