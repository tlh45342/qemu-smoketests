CROSS   ?= arm-none-eabi-
CC      := $(CROSS)gcc
LD      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

CFLAGS  := -mcpu=arm926ej-s -marm -ffreestanding -fno-builtin -Wall -Wextra -O2
LDFLAGS := -nostdlib -nostartfiles -Wl,-T,linker.ld

QEMU    ?= qemu-system-arm
QEMU_M  ?= versatilepb
QEMU_FLAGS ?= -M $(QEMU_M) -nographic -kernel timer.elf

# For test automation
TEST_TIMEOUT ?= 14
TEST_LOG     ?= qemu_output.txt
QEMU_PIDFILE ?= qemu.pid

.PHONY: all clean run run-bg kill test

all: timer.elf timer.bin

timer.elf: startup.o main.o linker.ld
	$(LD) $(LDFLAGS) -o $@ startup.o main.o

startup.o: startup.S
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

timer.bin: timer.elf
	$(OBJCOPY) -O binary $< $@

run: timer.elf
	$(QEMU) $(QEMU_FLAGS)

# Run QEMU in the background (writes PID to qemu.pid, output to qemu_output.txt)
run-bg: timer.elf
	@echo "Starting QEMU in background..."
	@rm -f $(QEMU_PIDFILE) $(TEST_LOG)
	@($(QEMU) $(QEMU_FLAGS) > $(TEST_LOG) 2>&1 & echo $$! > $(QEMU_PIDFILE))
	@echo "PID: $$(cat $(QEMU_PIDFILE))"
	@echo "Log: $(TEST_LOG)"

# Best-effort kill of background QEMU
kill:
	@if [ -f "$(QEMU_PIDFILE)" ]; then \
		pid=$$(cat "$(QEMU_PIDFILE)"); \
		echo "Killing QEMU PID $$pid"; \
		kill $$pid 2>/dev/null || true; \
		rm -f "$(QEMU_PIDFILE)"; \
	else \
		echo "No $(QEMU_PIDFILE) found."; \
	fi

# Automated smoke test:
# - run QEMU for TEST_TIMEOUT seconds
# - verify we saw START and at least one A and B
test: timer.elf
	@echo "Running QEMU for $(TEST_TIMEOUT)s and checking UART output..."
	@rm -f $(TEST_LOG)
	@timeout $(TEST_TIMEOUT) $(QEMU) $(QEMU_FLAGS) > $(TEST_LOG) 2>&1 || true
	@grep -q "START" $(TEST_LOG) || (echo "FAIL: missing START"; exit 1)
	@grep -q "A"     $(TEST_LOG) || (echo "FAIL: missing A"; exit 1)
	@grep -q "B"     $(TEST_LOG) || (echo "FAIL: missing B"; exit 1)
	@echo "PASS: saw START + A + B"
	@echo "Log saved to $(TEST_LOG)"

clean:
	rm -f *.o timer.elf timer.bin $(TEST_LOG) $(QEMU_PIDFILE)
