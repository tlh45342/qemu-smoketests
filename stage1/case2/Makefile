# Makefile — PL110 framebuffer demo for QEMU versatilepb
#
# Build:
#   make
# Run:
#   make run

TARGET   := pl110_box
SRC      := $(TARGET).c
OBJ      := $(TARGET).o
ELF      := $(TARGET).elf
BIN      := $(TARGET).bin
LST      := $(TARGET).lst
MAP      := $(TARGET).map

CROSS    ?= arm-none-eabi-
CC       := $(CROSS)gcc
OBJCOPY  := $(CROSS)objcopy
OBJDUMP  := $(CROSS)objdump

# versatilepb is ARM926EJ-S (ARMv5)
CPUFLAGS := -mcpu=arm926ej-s -marm

# QEMU -kernel loader expects the image linked at 0x00010000 :contentReference[oaicite:6]{index=6}
LOADADDR := 0x00010000

CFLAGS   := -std=c11 -Wall -Wextra -O2 \
            -ffreestanding -fno-builtin -fno-pic -fno-stack-protector \
            $(CPUFLAGS)

LDFLAGS  := -nostdlib \
            -Wl,-Ttext=$(LOADADDR) \
            -Wl,-e,_start \
            -Wl,-Map=$(MAP) \
            -Wl,--build-id=none

QEMU     ?= qemu-system-arm
QEMUFLAGS?= -M versatilepb -m 128M -display gtk

ifeq ($(OS),Windows_NT)
RM_F ?= del /q
else
RM_F ?= rm -f
endif

all: $(BIN)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(LST): $(ELF)
	$(OBJDUMP) -d $< > $@

run: $(BIN)
	$(QEMU) $(QEMUFLAGS) -kernel $(BIN)

clean:
	-@$(RM_F) $(OBJ) $(ELF) $(BIN) $(LST) $(MAP)

.PHONY: all run clean