# VersatilePB Timer IRQ Demo (Stage 1 / Case 5)

This project is a **bare-metal ARM** example designed to run on **QEMU** using:

- `qemu-system-arm -M versatilepb -nographic`

It demonstrates the “Stage 1 / Case 5” proof:

✅ Setting up **mode-specific stacks** (SVC + IRQ)  
✅ Installing an **interrupt vector table**  
✅ Configuring the **VIC (PL190)** interrupt controller  
✅ Programming the **SP804 dual timer** to generate periodic interrupts  
✅ Handling the IRQ and dispatching to a C “process”  
✅ Printing alternating `A` / `B` on **UART0** every **5 seconds** (so each letter is 10s apart, offset by 5s)

---

## What you should see

When you run it, the UART output should look like:

START
A B A B A B ...

markdown
Copy code

(There may be no spaces/newlines unless you add them. The important part is the *timed alternating characters*.)

---

## File layout

- `startup.S`  
  - Vector table at address 0  
  - Sets up **SVC stack** and **IRQ stack**
  - Provides IRQ wrapper (save state → call C handler → restore state → return)

- `main.c`  
  - UART0 init + `uart_putc()`
  - VIC init (enable timer IRQ line)
  - Timer init for a ~5 second periodic interrupt
  - C IRQ handler prints alternating `A` / `B`

- `linker.ld`  
  - Places vectors/text at start of RAM
  - Defines stack tops near end of RAM

- `Makefile`  
  - Builds `timer.elf` and `timer.bin`

---

## Prerequisites

You need:

- `qemu-system-arm`
- ARM embedded toolchain (one of these):
  - `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, etc.

On many Linux distros you can install:
- `gcc-arm-none-eabi`
- `qemu-system-arm`

---

## Build

```bash
make
Outputs:

timer.elf (run this in QEMU)

timer.bin (optional raw binary)

Run in QEMU
bash
Copy code
qemu-system-arm -M versatilepb -nographic -kernel timer.elf
Notes:

-nographic routes UART0 to your terminal.

-kernel timer.elf loads the ELF and starts executing at _start.

How it works (high level)
1) Startup / stacks / vectors
startup.S:

installs the vector table at 0x00000000

sets sp in SVC mode

switches to IRQ mode and sets a separate sp

returns to SVC and calls c_startup()

2) Timer IRQ
main.c configures the SP804 timer:

loads a tick count corresponding to ~5 seconds

enables periodic mode + interrupt generation

3) VIC routing
main.c enables the timer interrupt line in the PL190 VIC:

ensures it is routed as an IRQ (not FIQ)

sets the enable bit in VICIntEnable

4) ISR flow
CPU takes IRQ, branches to the IRQ vector

assembly wrapper saves registers and calls irq_handler_c()

C handler clears the timer interrupt

prints A or B and toggles state

signals end-of-interrupt to the VIC

Changing the timing
The demo uses a periodic “tick” intended to be ~5 seconds:

c
Copy code
const uint32_t ticks = 5000000u;  // ~5 seconds at 1 MHz
If your QEMU build/device clocking results in a different rate, adjust ticks:

1 second ≈ 1,000,000

2 seconds ≈ 2,000,000

5 seconds ≈ 5,000,000

10 seconds ≈ 10,000,000

Common troubleshooting
No output
Make sure you ran with -nographic.

Verify toolchain is building for arm926ej-s.

IRQ never fires
Ensure IRQs are enabled (CPSR I-bit cleared).

Ensure timer interrupt is enabled in timer control register.

Ensure VIC line is enabled and routed as IRQ.

Output prints too fast / too slow
Adjust the ticks constant.

Clean
bash
Copy code
make clean
Next upgrades (Stage 1 follow-ons)
Add a simple “scheduler” list: multiple timer callbacks

Use Timer0 and Timer1 simultaneously (true dual-timer offset model)

Add an IRQ-driven “heartbeat” + main loop work

Add VIC vectored interrupts instead of a single shared handler

makefile
Copy code
