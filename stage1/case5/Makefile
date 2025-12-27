# case1/Makefile — putchars_uart_c.c (no startup.s, no linker.ld)
#
# Builds:
#   firmware.elf  (ELF image)
#   firmware.bin  (raw binary for -kernel)
#
# Runs (UART on terminal):
#   qemu-system-arm -M versatilepb -m 128M -nographic -serial mon:stdio -kernel firmware.bin

# ----------------------------
# Toolchain
# ----------------------------
CROSS   ?= arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE    := $(CROSS)size

# ----------------------------
# Project
# ----------------------------
PROG ?= firmware
SRC  ?= putchars_uart_c.c
OBJ  := $(SRC:.c=.o)

# Where QEMU versatilepb loads a raw -kernel image by default:
LOAD_ADDR ?= 0x00010000

CPU    ?= arm926ej-s
CFLAGS ?= -mcpu=$(CPU) -marm -ffreestanding -fno-builtin \
          -Wall -Wextra -O2 -nostdlib -nostartfiles
# Place .text at LOAD_ADDR and set entry point to _start (your C file defines it)
LDFLAGS ?= -Wl,-Ttext=$(LOAD_ADDR) -Wl,-e,_start -Wl,-Map,$(PROG).map \
           -nostdlib -nostartfiles

# ----------------------------
# QEMU run config
# ----------------------------
QEMU    ?= qemu-system-arm
MACHINE ?= versatilepb
RAM     ?= 128M

# If you run from MSYS2 mintty and stdio acts up, try:
#   make run WINPTY=1
WINPTY ?= 0

ifeq ($(WINPTY),1)
QEMU_RUN := winpty $(QEMU)
else
QEMU_RUN := $(QEMU)
endif

# ----------------------------
# Windows-friendly rm fallback
# ----------------------------
ifeq ($(OS),Windows_NT)
  ifneq (,$(wildcard C:/Program Files/GNU/bin/rm.exe))
    RM_F := "C:/Program Files/GNU/bin/rm.exe" -f
  else ifneq (,$(wildcard C:/msys64/usr/bin/rm.exe))
    RM_F := C:/msys64/usr/bin/rm.exe -f
  else ifneq (,$(wildcard C:/cygwin64/bin/rm.exe))
    RM_F := C:/cygwin64/bin/rm.exe -f
  else
    RM_F := cmd /c del /q
  endif
else
  RM_F := rm -f
endif

# ----------------------------
# Targets
# ----------------------------
.PHONY: all clean run run_elf disasm info

all: $(PROG).elf $(PROG).bin info

$(PROG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(PROG).bin: $(PROG).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# UART + monitor in same terminal (no stdio collision)
run: $(PROG).bin
	$(QEMU_RUN) -M $(MACHINE) -m $(RAM) -nographic -serial mon:stdio -kernel $(PROG).bin

# QEMU can also load the ELF directly (sometimes handy)
run_elf: $(PROG).elf
	$(QEMU_RUN) -M $(MACHINE) -m $(RAM) -nographic -serial mon:stdio -kernel $(PROG).elf

disasm: $(PROG).elf
	$(OBJDUMP) -d $(PROG).elf | less

info: $(PROG).elf
	@$(SIZE) $(PROG).elf || true
	@echo "LOAD_ADDR=$(LOAD_ADDR)"

clean:
	@echo Cleaning...
	-@$(RM_F) *.o *.elf *.bin *.map 2>nul