# qemu-smoketests

A small, growing collection of **bare-metal “smoke tests”** that run under **QEMU**.  
Each **Stage** contains a sequence of **Cases** that prove one concrete capability at a time (UART, timers, IRQs, framebuffer, disk/MMIO, etc.).

The goal is simple: **build confidence in the minimum kernel bring-up path** by verifying each subsystem in isolation, then stacking the proofs.

---

## Repository layout

qemu-smoketests/
stage1/
case1/
case2/
case3/
stage2/
case1/
case2/
stage3/

markdown
Copy code

- **Stages** represent increasing capability levels (bring-up → interrupts → graphics → storage → …).
- **Cases** are small, self-contained programs that demonstrate one proof and are easy to run and verify.

---

## Conventions

### Case directory contract
Each `stageX/caseY/` should contain (at minimum):

- `README.md` (what it proves, how to run, expected output)
- `Makefile` (supports `make`, `make run`, `make test`, `make clean`)
- Source and link files (typical):
  - `startup.S` (vectors + stack setup)
  - `main.c`
  - `linker.ld`

### Output contract
Each case should print a recognizable banner on UART at boot, e.g.:

- `STAGE1 CASE5 START`

This makes automated “smoke testing” trivial (`grep` for the banner).

### Independence
Cases should be as independent as possible:
- No hidden dependencies on other cases
- No shared build state
- Minimal shared code unless it genuinely reduces repetition (later we can add `common/`)

---

## Toolchain requirements

You will generally need:

- `qemu-system-arm`
- An ARM bare-metal cross compiler (typical):
  - `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, etc.

If you’re targeting the QEMU `versatilepb` machine (common early bring-up target), your builds will typically use `-mcpu=arm926ej-s` and run with:

- `qemu-system-arm -M versatilepb -nographic ...`

---

## Quickstart (run a single case)

From a case directory:

```bash
cd stage1/case1
make
make run
If the case supports an automated run:

bash
Copy code
make test
make run is usually interactive (Ctrl+C to stop).
make test usually runs QEMU briefly and verifies output.

Running everything
This repo is designed so you can eventually run all smoke tests with a single command from the root.

Typical target shape (once the root Makefile exists):

bash
Copy code
make test
If you don’t have a root Makefile yet, you can still run per-case tests with:

bash
Copy code
make -C stage1/case2 test
Stages: intent and scope
Stage 1 — Bring-up proofs
Establish the minimum working environment:

reset → vectors → stacks

UART output

basic memory sanity (BSS clear, simple loops)

simple interrupts (timer IRQ to UART)

Example proofs:

UART hello

timer interrupt prints a pattern

correct mode stacks (SVC/IRQ) demonstrated

Stage 2 — Expanding hardware interaction
Add additional subsystems or more realistic service loops:

multiple IRQ sources / dispatch

basic device MMIO beyond UART/timer

early graphics experiments (framebuffer rectangles)

improved debug/exception reporting

Stage 3 — Higher-level proofs
Layer toward “kernel-ish” behavior:

simple scheduler/tick services

input events

storage I/O smoke tests (block read)

simple file parsing / FAT probing

stronger harness + regression

How to add a new Case
Copy an existing case directory as a template.

Update:

the banner string (so tests can uniquely identify it)

the case README.md (what it proves + expected output)

Keep the case small:

one proof

obvious output

deterministic behavior

Philosophy
Tiny programs, clear proofs

Fast to run, easy to verify

Each case stands alone

Output is part of the test harness

License
Add your license of choice (MIT/BSD/Apache-2.0/etc.) when you publish the repo.

pgsql
Copy code

If you want, I can also generate a matching **root `Makefile`** that automatically discovers `stage*/case*/` directories and runs `make test` in each one, producing a clean summary (PASS/FAIL per case).
::contentReference[oaicite:0]{index=0}