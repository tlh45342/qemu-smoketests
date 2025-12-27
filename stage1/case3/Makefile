CROSS   ?= arm-none-eabi
CC      := $(CROSS)-gcc
AS      := $(CROSS)-as
OBJCOPY := $(CROSS)-objcopy

CFLAGS  := -Wall -O2 -ffreestanding -nostdlib -nostartfiles -mcpu=arm926ej-s

OBJS    := startup.o main.o
BIN     := case3.bin

QEMU      ?= qemu-system-arm
QEMUFLAGS ?= -M versatilepb -m 128M -display gtk -serial stdio

all: $(BIN)

startup.o: startup.s
	$(AS) -mcpu=arm926ej-s -o $@ $<

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

case3.elf: $(OBJS)
	$(CC) $(CFLAGS) \
	    -Wl,-Ttext=0x00010000 \
	    -o $@ $(OBJS)

$(BIN): case3.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o case3.elf case3.bin

run: $(BIN)
	$(QEMU) $(QEMUFLAGS) -kernel $(BIN)
