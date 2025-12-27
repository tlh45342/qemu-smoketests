# case4/Makefile
CROSS ?= arm-none-eabi-
CC    := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

CFLAGS := -mcpu=arm926ej-s -marm -ffreestanding -fno-builtin \
          -Wall -Wextra -O2 -nostdlib -nostartfiles
LDFLAGS := -T linker.ld -Wl,-Map,case4.map

OBJS := start.o main.o gfx.o

all: case4.elf case4.bin

start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c pl110.h gfx.h icon_spinner.h
	$(CC) $(CFLAGS) -c main.c -o main.o

gfx.o: gfx.c gfx.h
	$(CC) $(CFLAGS) -c gfx.c -o gfx.o

case4.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

case4.bin: case4.elf
	$(OBJCOPY) -O binary $< $@

run: case4.elf
	qemu-system-arm -M versatilepb -m 128M -serial stdio -kernel case4.elf

clean:
	rm -f *.o *.elf *.bin *.map

.PHONY: all run clean